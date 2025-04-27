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

	// ...
}

void UCreDraFsm::DraIdle()
{
	Dragon->AttachToActor(TestPlayer,FAttachmentTransformRules::KeepRelativeTransform);
	
	MyTimer(&UCreDraFsm::NextState,1);
	Dragon->SetActorLocation(TestPlayer->GetActorLocation()+FVector(0,125,125));
}

void UCreDraFsm::DraAround()
{
	Dragon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	if (!TestPlayer) return;
	RotateTime += GetWorld()->GetDeltaSeconds() * 100;
	FRotator Rot = FRotator(0,RotateTime,0);
	// UE_LOG(LogTemp, Warning, TEXT("UCreDraFsm::DraAround RotTime %f 회전벡터 X %.2f Y %.2f Z %.2f"),RotateTime,Rot.Vector().X,Rot.Vector().Y,Rot.Vector().Z);
	
	Dragon->SetActorLocation(TestPlayer->GetActorLocation() + Rot.Vector() * 500);
	MyTimer(&UCreDraFsm::NextState,1);
}

void UCreDraFsm::DraAttack()
{
	MyTimer(&UCreDraFsm::NextState,10);
	
}

void UCreDraFsm::MyTimer(void(UCreDraFsm::*Func)(), int32 time = 2.0f)
{
	if (Func)
	{
		CurrentTime += GetWorld()->GetDeltaSeconds();
		if (CurrentTime > time)
		{
			(this->*Func)(); 
			UE_LOG(LogTemp, Display, TEXT("CreDraFsm 현 DraState는 %s"),
				*UEnum::GetValueAsString(EDraState(static_cast<int32>(Drastate))));
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

