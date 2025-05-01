// Fill out your copyright notice in the Description page of Project Settings.


#include "MonFsm.h"

#include "Monster.h"


// Sets default values for this component's properties
UMonFsm::UMonFsm()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMonFsm::BeginPlay()
{
	Super::BeginPlay();
	Monster = Cast<AMonster>(GetOwner());
	
	TargetLocation = RandLocation();
}


// Called every frame
void UMonFsm::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (MonState)
	{
	case EMonState::Idle:
		Idle();
		break;
	case EMonState::Around:
		Around();
		break;
	case EMonState::Attack:
		Attack();
		break;
	}

	// ...
}

void UMonFsm::Idle()
{
	CurrentTime += GetWorld()->GetDeltaSeconds();
	
	FVector distVec = TargetLocation - Monster -> GetActorLocation();
	float distLength = distVec.Length();
	Monster->AddActorWorldOffset(distVec.GetSafeNormal() * 10);
	
	if (CurrentTime > 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("2초 타이머 거리는 %f"), distLength);
		
		if (distLength < 100.f)
		{
			TargetLocation = RandLocation();
		}
		CurrentTime = 0.0f;
	}
}

void UMonFsm::Around()
{
	
}

void UMonFsm::Attack()
{
}

FVector UMonFsm::RandLocation(float X, float Y, float Z)
{
	float x = FMath::RandRange(-X, X);
	float y = FMath::RandRange(-Y, Y);
	float z = FMath::RandRange(0.0f, Z);

	FVector RandLoc = Monster->GetActorLocation() + FVector(x, y, z);

	return FVector(RandLoc);
}


