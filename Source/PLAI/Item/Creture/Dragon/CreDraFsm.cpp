// Fill out your copyright notice in the Description page of Project Settings.


#include "CreDraFsm.h"

#include "CreDragon.h"
#include "MovieSceneTracksComponentTypes.h"
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

	switch (Drastate)
	{
	case EDraState::DraIdle:
		DraIdle();
		break;

	case EDraState::DraAround:
		DraAround();
		break;

	case EDraState::DraAttack:
		DraAttack();
		break;
	}

	// if (bTimer == true)
	// {
	// 	UE_LOG(LogTemp,Warning,TEXT("CreDraFsm bool값은? %s"),bTimer ? TEXT("참") : TEXT("거짓"));
	// }

	// ...
}

void UCreDraFsm::DraIdle()
{
	// Dragon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	// Dragon->AttachToActor(TestPlayer,FAttachmentTransformRules::KeepRelativeTransform);
	
	MyTimer(&UCreDraFsm::NextState,0.1);
	// Dragon->SetActorLocation(TestPlayer->GetActorLocation()+FVector(0,125,125));
}

void UCreDraFsm::DraAround()
{
	Dragon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	// if (!TestPlayer) return;
	// RotateTime += GetWorld()->GetDeltaSeconds() * 100;
	// FRotator Rot = FRotator(0,RotateTime,0);
	// float Sine = sin(RotateTime);
	// Dragon->SetActorLocation(TestPlayer->GetActorLocation() + Rot.Vector() * 500
	// 	+FVector(0,0,200 + sin(RotateTime/30) * 100));
	
	if (bTimer)
	{
		PatrolPoints.Empty();
		FVector Loc = TestPlayer->GetActorLocation();
		for (int32 i = 0; i < 7; i++)
		{
			float x = FMath::RandRange(-1000,1000);
			float y = FMath::RandRange(-1000,1000);   
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

void UCreDraFsm::DraAttack()
{
	DrawDebugCircle(GetWorld(),PatrolPoints[PatrolIndex],50,10,FColor::Red,false,0.1);
	DrawDebugLine(GetWorld(),Dragon->GetActorLocation(),PatrolPoints[PatrolIndex],FColor::Red,false);
	
	Dragon->AddActorLocalOffset(FVector(1,0,0) * 30);
	
	UE_LOG(LogTemp,Warning,TEXT("CreDraFsm %f"),
		FVector::Dist(PatrolPoints[PatrolIndex] , Dragon->GetActorLocation()));
	if (FVector::Dist(PatrolPoints[PatrolIndex] , Dragon->GetActorLocation()) < 50)
	{
		UE_LOG(LogTemp,Warning,TEXT("CreDraFsm Patrol넘어감 %d 위치는 X 0.2%f 0.2%f Z 0.2Y%f"),PatrolIndex
			,PatrolPoints[PatrolIndex].X,PatrolPoints[PatrolIndex].Y,PatrolPoints[PatrolIndex].Z);
		
		PatrolIndex++;
		FVector dir = PatrolPoints[PatrolIndex] - Dragon->GetActorLocation();
		dir.Normalize();
		Dragon->SetActorRotation(dir.Rotation());
		
		if (PatrolIndex > PatrolPoints.Num()-1 || bTimer == true)
		{
			PatrolIndex = 0;
		}
	}
	MyTimer(&UCreDraFsm::NextState,3);
}

void UCreDraFsm::MyTimer(void(UCreDraFsm::*Func)(), int32 time = 2.0f)
{
	bTimer = false;
	if (Func)
	{
		CurrentTime += GetWorld()->GetDeltaSeconds();
		if (CurrentTime > time)
		{
			(this->*Func)(); 
			UE_LOG(LogTemp, Display, TEXT("CreDraFsm 현 DraState는 %s"),
				*UEnum::GetValueAsString(EDraState(static_cast<int32>(Drastate))));
			bTimer = true;
			CurrentTime = 0.0f;
		}
	}
}

void UCreDraFsm::NextState()
{
	if (Drastate != EDraState::DraAttack)
	{
		Drastate = static_cast<EDraState>(static_cast<int32>(Drastate) + 1);
	}
	else
	{
		Drastate = static_cast<EDraState>(0);
	}
	
	// if (Drastate != EDraState::DraAttack)
	// {
	// 	Drastate = EDraState(static_cast<int32>(Drastate)+1);
	// }
	// else
	// {
	// 	Drastate = EDraState(0);
	// }
}

