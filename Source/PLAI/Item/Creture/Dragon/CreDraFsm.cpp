// Fill out your copyright notice in the Description page of Project Settings.


#include "CreDraFsm.h"

#include "CreDragon.h"
#include "MovieSceneTracksComponentTypes.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "RewindData.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PLAI/Item/Monster/MonFsm.h"
#include "PLAI/Item/Monster/Monster.h"
#include "PLAI/Item/Monster/MonsterMaster.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"


// Sets default values for this component's properties
UCreDraFsm::UCreDraFsm()
{
	PrimaryComponentTick.bCanEverTick = true;
	TimerMultiPre.SetNum(3);
}

void UCreDraFsm::BeginPlay()
{
	Super::BeginPlay();

	Dragon = Cast<ACreDragon>(Creature);
	if (Dragon) { UE_LOG(LogTemp, Warning, TEXT("CreDraFsm = Created Dragon")); }
	else { UE_LOG(LogTemp, Warning, TEXT("CreDraFsm = Created Dragon")); }
}

void UCreDraFsm::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// DrawDebugString(GetWorld(),TestPlayer->GetActorLocation() + FVector(0, 0, 150),
	// FString::Printf(TEXT("CreDreFsm DraState 현재[%s]"),*UEnum::GetValueAsString(EDraState(static_cast<int32>
	// 	(Drastate)))),nullptr,FColor::Red,0.f,false);
	
	switch (Drastate)
	{
	case EDraState::DraIdle:
		DraIdle(1);
		break;
	case EDraState::DraAround:
		DraAround(3);
		break;
	
	case EDraState::DraAttackRangePre:
		DraAttackRangePre(1);
		break;
	case EDraState::DraAttackRange:
		DraAttackRange(3);
		break;
	case EDraState::DraAttackRangeFinish:
		DraAttackRangeFinish();
		break;
	
	case EDraState::DraAttackSingleRange:
		DraAttackSingleRange(3000,5.0f);
		break;
	
	case EDraState::DraAttackMultiPre:
		DraAttackMultiPre(1);
		break;
	case EDraState::DraAttackMulti:
		DraAttackMulti(3);
		break;
	}
}

void UCreDraFsm::DraIdle(float time)
{
	if (!TestPlayer) return;
	// if (CurrentTime - GetWorld()->GetDeltaSeconds() <= 0.f){} 최초한번 실행됨
	Dragon->AttachToActor(TestPlayer,FAttachmentTransformRules::KeepRelativeTransform);
	Dragon->SetActorLocation(TestPlayer->GetActorLocation()+FVector(0,125,125));
	
	CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime < time) return;
	CurrentTime = 0;
	
	Dragon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	Drastate = EDraState::DraAround;
}

void UCreDraFsm::DraAround(float time)
{
	if (!TestPlayer) return;
	RotateTime += GetWorld()->GetDeltaSeconds();
	FRotator Rot = FRotator(0,RotateTime * 90,0);
	Dragon->SetActorLocation(TestPlayer->GetActorLocation() + Rot.Vector() * 500
		+FVector(0,cos(RotateTime * 3) * 200,200 + sin(RotateTime * 10) * 100));

	if (GetMonsterBySphere(Creature,2500).Monsters.Num() > 0)
	{
		UE_LOG(LogTemp,Warning,TEXT("CreDreFsm 순찰중 사냥모드 진입"))
		Drastate = EDraState::DraAttackRangePre;
	}
}

void UCreDraFsm::DraAttackRangePre(float time)
{
	PatrolPoints.Empty();
	FVector Loc = TestPlayer->GetActorLocation();
	for (int32 i = 0; i < 10; i++)
	{
		float x = FMath::RandRange(-750,750);
		float y = FMath::RandRange(-750,750);   
		float z = FMath::RandRange(0,500);
		PatrolPoints.Add(Loc + FVector(x,y,z));
		// if (i == 0)
		// { DrawDebugSphere(GetWorld(),PatrolPoints[i],75,20,FColor::Black,false,10); }
		// else
		// { DrawDebugSphere(GetWorld(),PatrolPoints[i],50,10,FColor::Red,false,10); }
	}
	FVector dir = PatrolPoints[0] - Dragon->GetActorLocation();
	dir.Normalize();
	Dragon->SetActorRotation(dir.Rotation());
	Drastate = EDraState::DraAttackRange;
}

void UCreDraFsm::DraAttackRange(float time)
{
	int32 PatrolIndex = 0;
	FVector Dir = PatrolPoints[0] - Dragon->GetActorLocation();
	Dir.Normalize();
	Dragon->AddActorWorldOffset(Dir * 100);
	
	if (FVector::Dist(PatrolPoints[0] , Dragon->GetActorLocation()) < 50)
	{
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),
		NiagaraSkills[3],LineTraceZ(Creature,Dragon->GetActorLocation()),
		FRotator::ZeroRotator,FVector(1.f),true,true,ENCPoolMethod::AutoRelease);
		NiagaraComp->SetActive(true);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ParticlesSkills[0],
		   LineTraceZ(Creature,Dragon->GetActorLocation()),
		   FRotator(0,0,0),/*Scale=*/ FVector(1.f),/*bAutoDestroy=*/ true);
		
		for (AMonster* Monster : GetMonsterBySphere(Creature,300).Monsters)
		{
			// UE_LOG(LogTemp,Warning,TEXT())
			AttackMonster(Monster);
			// DrawDebugSphere(GetWorld(),Monster->GetActorLocation(),300,
			// 	100,FColor::Black,false,.15); 
		}
		FVector dir = PatrolPoints[0] - Dragon->GetActorLocation();
		dir.Normalize();
		Dragon->SetActorRotation(dir.Rotation());
		PatrolPoints.RemoveAt(0);
		if (PatrolPoints.Num() == 0)
		{
			Dragon->SetActorLocation(Dragon->GetActorLocation() + FVector(0,0,3000));
			Drastate = EDraState::DraAttackRangeFinish;
			return;
		}
		// { Drastate = EDraState::DraAttackSingleRange; return;}
	}
	CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime < time) return;
	if (GetMonsterBySphere(Creature,2000).Monsters.Num() == 0) { Drastate = EDraState::DraIdle;}
	CurrentTime = 0;
}

void UCreDraFsm::DraAttackRangeFinish(float time)
{
	CurrentTime += GetWorld()->GetDeltaSeconds();
	Dragon->AddActorWorldOffset(FVector(0,0,-50));
	Dragon->SetActorRotation(FVector(0,0,-1).Rotation());
	Dragon->AddActorWorldRotation(FRotator(0,CurrentTime * 360,0));
	if (FVector::Distance(Dragon->GetActorLocation(),LineTraceZ(Creature,Dragon->GetActorLocation())) < 50)
	{
		Drastate = EDraState::DraAttackSingleRange;	CurrentTime = 0;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ParticlesSkills[1],
		   LineTraceZ(Creature,Dragon->GetActorLocation()),
		   FRotator(0,0,0),/*Scale=*/ FVector(3.f),/*bAutoDestroy=*/ true);
		for (AMonster* Monster : GetMonsterBySphere(Creature,800).Monsters)
		{ AttackMonster(Monster); }
	}
}

void UCreDraFsm::DraAttackSingleRange(float Radios, float time)
{
	if (PlayerDistance() > 2800 || GetMonsterBySphere(Creature,2800).Monsters.Num() == 0)
	{ Drastate = EDraState::DraIdle; }
	
	Dragon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime > time)
	{
		Drastate = EDraState::DraAttackMultiPre;
		CurrentTime = 0;
	}
	AMonster* NearMonster = nullptr;
	
	TimeFire += GetWorld()->GetDeltaSeconds();
	if (TimeFire < 1) return;
	TimeFire = 0.0f;
	
	TArray<FOverlapResult>Results;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Dragon);
	
	bool bHit = GetWorld()->OverlapMultiByChannel(Results,Dragon->GetActorLocation(),
		FQuat::Identity,ECC_Pawn,FCollisionShape::MakeSphere(Radios),Params);
	if (bHit)
	{
		float Distance = 10000.0f;
		for (FOverlapResult Result: Results)
		{
			AMonster* Monster = Cast<AMonster>(Result.GetActor());
			if (Monster)
			{
				float CurDist = FVector::Distance(Dragon->GetActorLocation(),Result.GetActor()->GetActorLocation());
				if (CurDist < Distance)
				{
					Distance = CurDist;
					NearMonster = Monster;
				}
			}
		}
		if (NearMonster)
		{
			if (ACreBullet* Bullet = GetWorld()->SpawnActor<ACreBullet>(CreBulletFactory))
			{
				Bullet->SetActorLocation(Dragon->GetActorLocation()+Dragon->GetActorForwardVector() * 75);
				FVector dist = Dragon->GetActorLocation() - NearMonster->GetActorLocation();
				Bullet->ProjectileComp->Velocity = dist.GetSafeNormal() * -2000;
			}
			else
			{
				UE_LOG(LogTemp,Warning,TEXT("CreDraFsm Bullet 없음"))
			}
			AttackMonster(NearMonster);
			// NearMonster->MonsterStruct.CurrentHp -= Dragon->CreFsm->CreStruct.Atk;
			// NearMonster->SetHpBar();
			// NearMonster->DamageUi(CreatureDamage());
			// if (NearMonster->MonsterStruct.CurrentHp <= 0)
			// {
			// 	CreStruct.CurrentExp += NearMonster->MonsterStruct.Exp;
			// 	NearMonster->Dead();
			// 	SetCreStat();
			// 	GetMonGold(NearMonster);
			// }
			UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),
	        NiagaraSkills[1],NearMonster->GetActorLocation(),FRotator::ZeroRotator,FVector(1.f),true, // AutoActivate                        
	        true,// AutoDestroy
	        ENCPoolMethod::AutoRelease); NiagaraComp->SetActive(true);
			
			FVector dist = NearMonster->GetActorLocation() - Dragon->GetActorLocation();
			Dragon->SetActorRotation(dist.GetSafeNormal().Rotation());
		}
	}
}

void UCreDraFsm::DraAttackMultiPre(float time, float Radius)
{
	if (PlayerDistance() > 2500 || GetMonsterBySphere(Creature,2500).Monsters.Num() == 0)
	{ Drastate = EDraState::DraIdle; }
	
	// UE_LOG(LogTemp,Warning,TEXT("CreDraFsm 멀티공격준비 초시계%f"),CurrentTime)
	CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime < time)return;
	CurrentTime = 0; // 밑에거 한번만 실행됨

	Monsters.Empty();
	//Remove 해줘야할듯
	TArray<FOverlapResult>HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Dragon); 
	bool bHit = GetWorld()->OverlapMultiByChannel(HitResults,Dragon->GetActorLocation(),
		FQuat::Identity,ECC_Pawn,FCollisionShape::MakeSphere(Radius),Params);
	if (bHit)
	{
		for (FOverlapResult Result : HitResults)
		{
			if (AMonster* Monster = Cast<AMonster>(Result.GetActor()))
			{
				if (!Monsters.Contains(Monster))
				{
					Monsters.Add(Monster);
					// UE_LOG(LogTemp,Warning,TEXT("CraDraFsm 몬스터 몇마리씩 담기노 %d 네임 %s"),Monsters.Num(),*Monster->GetName())
				}
			}
		}
	}
	Dragon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Drastate = EDraState::DraAttackMulti;
}

void UCreDraFsm::DraAttackMulti(float time)
{
	// UE_LOG(LogTemp,Warning,TEXT("CreDraFsm 멀티공격준비 ->원거리공격 초시계%f"),TimerMulti)
	TimerMulti += GetWorld()->GetDeltaSeconds();
	if (TimerMulti > time)
	{
		TimerMulti = 0;
		Drastate = EDraState::DraAttackRangePre;
	}
	
	if (Monsters.Num() > 0)
	{
		if (FinishCount > 3)
		{
			FinishCount = 0;
			UE_LOG(LogTemp,Warning,TEXT("CraDraFsm 필살기 시전"))
		}
		AMonster* Monster = Monsters[0];
		if (IsValid(Monster))
		{
			FVector Dir = Monster->GetActorLocation() - Dragon->GetActorLocation();
			Dragon->AddActorWorldOffset(Dir.GetSafeNormal() * 70);
			Dragon->SetActorRotation(Dir.Rotation());
			if (FVector::Dist(Monster->GetActorLocation(),Dragon->GetActorLocation()) < 50.0f)
			{
				FinishCount++;
				AttackMonster(Monster);
				
				// Monster->MonsterStruct.CurrentHp -= Dragon->CreFsm->CreStruct.Atk;
				// Monster->SetHpBar();
				// Monster->DamageUi(CreatureDamage());
				//
				// if (Monster->MonsterStruct.CurrentHp <= 0)
				// {
				// 	CreStruct.CurrentExp += Monster->MonsterStruct.Exp;
				// 	Monster->Dead();
				// 	SetCreStat();
				// 	GetMonGold(Monster);
				// }
				
				Monsters.RemoveAt(0);
				int32 index = 0;
				FinishCount == 2 ? index = 2 : index = 1;  
				{
					UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),
				    NiagaraSkills[index],Monster->GetActorLocation(),FRotator::ZeroRotator,FVector(index),true, // AutoActivate                        
				    true,// AutoDestroy
				    ENCPoolMethod::AutoRelease); NiagaraComp->SetActive(true);
				}
			}
		}
	}
	else
	{
		Drastate = EDraState::DraAttackMultiPre;
	}
}




void UCreDraFsm::MyTimer(TFunction<void()> func, float time)
{
	if (!IsValid(Dragon)) 
	{
		UE_LOG(LogTemp, Error, TEXT("DraAround: Dragon 포인터가 유효하지 않습니다."));
		return;
	}
	bTimer = false;
	if (func)
	{
		CurrentTime += GetWorld()->GetDeltaSeconds();
		if (CurrentTime > time)
		{
			func(); 
			CurrentTime = 0.0f;
			bTimer = true;
		}
	}
}


void UCreDraFsm::NextState()
{
	if (Drastate != EDraState::DraAttackMulti)
	{ Drastate = static_cast<EDraState>(static_cast<int32>(Drastate) + 1); }
	else
	{ Drastate = static_cast<EDraState>(0); }
}
















//
// if (!Monsters.IsValidIndex(MultiCount))
// {
// 	UE_LOG(LogTemp, Error, TEXT("Invalid MultiCount: %d / Num:%d"), MultiCount, Monsters.Num());
// 	Drastate = EDraState::DraAttackMultiPre;
// 	return;
// }
// UE_LOG(LogTemp, Error, TEXT("Invalid MultiCount: 실행되는거맞나? %d / Num:%d"), MultiCount, Monsters.Num());
// FVector dist = Monsters[MultiCount]->GetActorLocation() - Dragon->GetActorLocation();
// Dragon->AddActorWorldOffset(dist.GetSafeNormal() * 20);
// Dragon->SetActorRotation(dist.GetSafeNormal().Rotation());
// 	
// if (dist.Length() < 50)
// {   DrawDebugSphere(GetWorld(),Dragon->GetActorLocation()+Dragon->GetActorForwardVector() * 50,
// 150,10,FColor::Red,false,1);
// 	
// 	FHitResult HitResult;
// 	FCollisionQueryParams Params;
// 	Params.AddIgnoredActor(Dragon);
// 	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult,Dragon->GetActorLocation(),
// 		Dragon->GetActorLocation() + Dragon->GetActorForwardVector() * 50,ECC_Visibility,Params);
// 	if (bHit)
// 	{
// 		UE_LOG(LogTemp,Warning,TEXT("CreDraFSm 누구랑 부딛힘? %s"),*HitResult.GetActor()->GetName())
// 		if (AMonster* Monster = Cast<AMonster>(HitResult.GetActor()))
// 		{
// 			UE_LOG(LogTemp,Warning,TEXT("CreDraFSm Monster 체력 깎음 %s"),*Monster->GetName())
// 			Monster->MonsterStruct.CurrentHp -= Dragon->ItemStructTable.ItemStructStat.item_ATK;
// 			Monster->SetHpBar();
// 		}
// 	}
// 	MultiCount++;
// 	FinishCount++;
// 	if (MultiCount >= Monsters.Num())
// 	{
// 		Dragon->AttachToActor(TestPlayer,FAttachmentTransformRules::KeepWorldTransform);
// 		Drastate = EDraState::DraAttackMultiPre;
// 	}
// 	if (FinishCount % 3 == 0)
// 	{
// 		FinishCount = 0;
// 		UE_LOG(LogTemp,Warning,TEXT("CraDraFsm 필살기 시전"));
// 	}
// }


// CurrentTime += GetWorld()->GetDeltaSeconds();
// if (CurrentTime < 2)
// {
// 	return;
// }
// CurrentTime = 0;
//
// AMonster* NearMonster = nullptr;
// TArray<FOverlapResult>Results;
// FCollisionQueryParams Params;
// Params.AddIgnoredActor(Dragon);
//
// FCollisionObjectQueryParams ObjectParams;
// //추후에 수정 크리처 가능성 콜리전 채널
// ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
// FCollisionShape ShapeParams = FCollisionShape::MakeSphere(2000);
//
// bool bHits = GetWorld()->OverlapMultiByObjectType(Results,Dragon->GetActorLocation()
// ,FQuat::Identity,ObjectParams,ShapeParams,Params);
//
// float MonDist = 10000.0f;
// if (bHits)
// {
// 	for (FOverlapResult Result : Results)
// 	{
// 		if (AMonster* Monster = Cast<AMonster>(Result.GetActor()))
// 		{
// 			float CurDist = FVector::Distance(Dragon->GetActorLocation(),Result.GetActor()->GetActorLocation());
//                if (CurDist < MonDist)
//                {
//                	MonDist = CurDist;
//                	NearMonster = Monster;
//                }
// 		}
// 	}
// 	if (NearMonster)
// 	{
// 		DrawDebugSphere(GetWorld(),NearMonster->GetActorLocation(),50,10,FColor::Blue,false,2);
// 		NearMonster->MonsterStruct.CurrentHp -= Dragon->ItemStructTable.ItemStructStat.item_ATK;
// 		NearMonster->SetHpBar();
// 	}
// }


// if (ACreDragon* Dra = Cast<ACreDragon>(GetOwner()))
// {
// 	UE_LOG(LogTemp, Warning, TEXT("UCreDraFsm::BeginPlay 드래곤있음"))
// 	Dragon = Dra;
// 	if (ACreature* Creature = Cast<ACreature>(Dragon->CreFsm->GetOwner()))
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("UCreDraFsm::BeginPlay Creature있음"))
// 	}
// 	else
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("UCreDraFsm::BeginPlay Creature없음"))
// 	}
// }
// else
// {
// 	UE_LOG(LogTemp, Warning, TEXT("UCreDraFsm::BeginPlay 드래곤없음"))
// }
// TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
// if (TestPlayer)
// {
// 	UE_LOG(LogTemp,Warning,TEXT("UCreDraFsm::BeginPlay PC->캐릭터 캐스팅 성공"));
// }
// else
// {
// 	UE_LOG(LogTemp,Warning,TEXT("UCreDraFsm::BeginPlay PC->캐릭터 캐스팅 실패"));
// }