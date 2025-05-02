// Fill out your copyright notice in the Description page of Project Settings.


#include "CreDraFsm.h"

#include "CreDragon.h"
#include "MovieSceneTracksComponentTypes.h"
#include "Engine/OverlapResult.h"
#include "PLAI/Item/Monster/MonFsm.h"
#include "PLAI/Item/Monster/Monster.h"
#include "PLAI/Item/Monster/MonsterMaster.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"


// Sets default values for this component's properties
UCreDraFsm::UCreDraFsm()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UCreDraFsm::BeginPlay()
{
	Super::BeginPlay();
	TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (TestPlayer)
	{
		UE_LOG(LogTemp,Warning,TEXT("UCreDraFsm::BeginPlay PC->캐릭터 캐스팅 성공"));
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("UCreDraFsm::BeginPlay PC->캐릭터 캐스팅 실패"));
	}
	if (ACreDragon* Dra = Cast<ACreDragon>(GetOwner()))
	{
		UE_LOG(LogTemp, Warning, TEXT("UCreDraFsm::BeginPlay 드래곤있음"))
		Dragon = Dra;
		if (ACreature* Creature = Cast<ACreature>(Dragon->CreFsm->GetOwner()))
		{
			UE_LOG(LogTemp, Warning, TEXT("UCreDraFsm::BeginPlay Creature있음"))
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UCreDraFsm::BeginPlay Creature없음"))
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UCreDraFsm::BeginPlay 드래곤없음"))
	}
}


// Called every frame
void UCreDraFsm::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DraAttack();
	
	// OverlappedSphere(2000,2.0f);
	
	// switch (Drastate)
	// {
	// case EDraState::DraIdle:
	// 	DraIdle();
	// 	break;
	//
	// case EDraState::DraAround:
	// 	DraAround();
	// 	break;
	//
	// case EDraState::DraPatrol:
	// 	DraPatrol();
	// 	break;
	//
	// case EDraState::DraAttack:
	// 	DraAttack();
	// 	break;
	// }

	// if (bTimer == true)
	// {
	// 	UE_LOG(LogTemp,Warning,TEXT("CreDraFsm bool값은? %s"),bTimer ? TEXT("참") : TEXT("거짓"));
	// }

	// ...
}

void UCreDraFsm::DraIdle()
{
	// Dragon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Dragon->AttachToActor(TestPlayer,FAttachmentTransformRules::KeepRelativeTransform);
	
	MyTimer(&UCreDraFsm::NextState,1);
	Dragon->SetActorLocation(TestPlayer->GetActorLocation()+FVector(0,125,125));
}

void UCreDraFsm::DraAround()
{
	RotateTime += GetWorld()->GetDeltaSeconds();
	Dragon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	
	if (!TestPlayer) return;
	
	FRotator Rot = FRotator(0,RotateTime * 90,0);
	
	Dragon->SetActorLocation(TestPlayer->GetActorLocation() + Rot.Vector() * 500
		
		+FVector(0,0,200 + sin(RotateTime * 10) * 100));
	
	if (bTimer)
	{
		PatrolPoints.Empty();
		FVector Loc = TestPlayer->GetActorLocation();
		for (int32 i = 0; i < 10; i++)
		{
			float x = FMath::RandRange(-750,750);
			float y = FMath::RandRange(-750,750);   
			float z = FMath::RandRange(200,500);
			PatrolPoints.Add(Loc + FVector(x,y,z));
			if (i == 0)
			{ DrawDebugSphere(GetWorld(),PatrolPoints[i],75,20,FColor::Black,false,10); }
			else
			{ DrawDebugCircle(GetWorld(),PatrolPoints[i],50,10,FColor::Red,false,10); }
			UE_LOG(LogTemp,Warning,TEXT("CreDraFsm 위치는 %0.2f,%0.2f,%0.2f"),
				PatrolPoints[i].X,PatrolPoints[i].Y,PatrolPoints[i].Z);
		}
		FVector dir = PatrolPoints[0] - Dragon->GetActorLocation();
		dir.Normalize();
		Dragon->SetActorRotation(dir.Rotation());
		
		DrawDebugLine(GetWorld(),Dragon->GetActorLocation(),
			PatrolPoints[0] ,FColor::Blue,false,3);

		DrawDebugCircle(GetWorld(),PatrolPoints[0] * 100,50,10,FColor::Blue,false,3);
	}
	MyTimer(&UCreDraFsm::NextState,2);
}

void UCreDraFsm::DraPatrol()
{
	if (PatrolPoints.Num() == 0 || PatrolIndex < 0 || PatrolIndex >= PatrolPoints.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("DraAttack: PatrolPoints 크기=%d, PatrolIndex=%d"), PatrolPoints.Num(), PatrolIndex);
		return;
	}
	DrawDebugCircle(GetWorld(),PatrolPoints[PatrolIndex],50,10,FColor::Red,false,0.1);
	DrawDebugLine(GetWorld(),Dragon->GetActorLocation(),PatrolPoints[PatrolIndex],FColor::Red,false);
	
	FVector Dir = PatrolPoints[PatrolIndex] - Dragon->GetActorLocation();
	Dir.Normalize();
	Dragon->AddActorWorldOffset(Dir * 25);
	
	if (FVector::Dist(PatrolPoints[PatrolIndex] , Dragon->GetActorLocation()) < 50)
	{
		PatrolIndex++;
		FVector dir = PatrolPoints[PatrolIndex] - Dragon->GetActorLocation();
		dir.Normalize();
		Dragon->SetActorRotation(dir.Rotation());
		
		if (PatrolIndex > PatrolPoints.Num()-2 || bTimer == true)
		{
			PatrolIndex = 0;
		}
	}
	MyTimer(&UCreDraFsm::NextState,10);
}

void UCreDraFsm::DraAttack(float Radius)
{
	CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime > 1.0f)
	{
		CurrentTime = 0.0f;
		TArray<FOverlapResult>HitResults;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Dragon);
		bool bHit = GetWorld()->OverlapMultiByObjectType(HitResults,Dragon->GetActorLocation(),
			FQuat::Identity,ECC_Pawn,FCollisionShape::MakeSphere(Radius),Params);
		float distclose = 5000.0f;
		if (bHit)
		{
			for (FOverlapResult Result : HitResults)
			{
				if (AMonster* Monster = Cast<AMonster>(Result.GetActor()))
				{
					if (distclose > FVector::Distance(Monster->GetActorLocation(),Dragon->GetActorLocation()))
					{
						distclose = FVector::Dist(Dragon->GetActorLocation(),Monster->GetActorLocation());
						Monsters.Add(Monster);
					}
				}
			}
		}
	}
}

void UCreDraFsm::MyTimer(void(UCreDraFsm::*Func)(), float time = 2.0f)
{
	if (!IsValid(Dragon)) 
	{
		UE_LOG(LogTemp, Error, TEXT("DraAround: Dragon 포인터가 유효하지 않습니다."));
		return;
	}
	bTimer = false;
	if (Func)
	{
		CurrentTime += GetWorld()->GetDeltaSeconds();
		if (CurrentTime > time)
		{
			(this->*Func)(); 
			UE_LOG(LogTemp, Display, TEXT("CreDraFsm 현 DraState는 %s"),
				*UEnum::GetValueAsString(EDraState(static_cast<int32>(Drastate))));
			
			CurrentTime = 0.0f;
			bTimer = true;
		}
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
	if (Drastate != EDraState::DraAttack)
	{ Drastate = static_cast<EDraState>(static_cast<int32>(Drastate) + 1); }
	else
	{ Drastate = static_cast<EDraState>(0); }
}

void UCreDraFsm::DraAttackSingleRange(float Radios, float time)
{
	AMonster* NearMonster = nullptr;

	TimeFire += GetWorld()->GetDeltaSeconds();
	if (TimeFire < time) return;
	TimeFire = 0.0f;
	
	TArray<FOverlapResult>Results;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Dragon);
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	bool bHit = GetWorld()->OverlapMultiByObjectType(Results,Dragon->GetActorLocation(),
		FQuat::Identity,ObjectParams,FCollisionShape::MakeSphere(Radios),Params);
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
			NearMonster->MonsterStruct.CurrentHp -= Dragon->ItemStructTable.ItemStructStat.item_ATK;
			NearMonster->SetHpBar();
			FVector dist = NearMonster->GetActorLocation() - Dragon->GetActorLocation();
			Dragon->SetActorRotation(dist.GetSafeNormal().Rotation());
			DrawDebugLine(GetWorld(),NearMonster->GetActorLocation(),NearMonster->GetActorLocation()+FVector(0,0,1000),
			FColor::Purple,false,1.5,0,1.5);
			DrawDebugSphere(GetWorld(),NearMonster->GetActorLocation(),50,
			15,FColor::Purple,false,1.5,0,2);
		}
	}
}







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
