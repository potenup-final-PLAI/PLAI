// Fill out your copyright notice in the Description page of Project Settings.


#include "MonFsm.h"

#include "MaterialHLSLTree.h"
#include "Monster.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"


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
	if (CurrentTime > 1)
	{
		MonState = EMonState::Around;
		LineDestination();
		InitLocation = TargetLocation;
		FVector Vec = TargetLocation = Monster->GetActorLocation();
		Monster->SetActorRotation(Vec.GetSafeNormal().Rotation());
		CurrentTime = 0;
	}
}

void UMonFsm::Around()
{
	MoveDestination();
}

void UMonFsm::Attack()
{
}

FVector UMonFsm::RandLocation(float X, float Y, float Z)
{
	float x = FMath::RandRange(-X, X);
	float y = FMath::RandRange(-Y, Y);
	float z = FMath::RandRange(0.0f, Z);
	return FVector(x,y,z);
}

void UMonFsm::MoveDestination()
{
	// DrawDebugLine(GetWorld(),Monster->GetActorLocation(),TargetLocation,FColor::Blue,false,0.02f);
    // DrawDebugSphere(GetWorld(),TargetLocation,20,30,FColor::Blue,false,0.02f);
	// Monster->SetActorRotation(Distance.GetSafeNormal().Rotation());

	FVector Distance = TargetLocation - Monster->GetActorLocation();
	Monster->AddActorWorldOffset(Distance.GetSafeNormal() * 10,false);

	if (Distance.Length() < 75)
	{
		LineDestination();
		if (FVector::Distance(TargetLocation,InitLocation) > InitLocationFloat)
		{
			FVector NewLoc = InitLocation - Monster->GetActorLocation();
			NewLoc.Normalize();
			TargetLocation = Monster->GetActorLocation() + NewLoc * 1500;
		}
		bRotator = true;
	}
	if (bRotator == true)
	{
		// UE_LOG(LogTemp,Warning,TEXT("MonFsm 초시계 %f"),CurrentTime)
		CurrentTime += GetWorld()->DeltaTimeSeconds;
		FRotator Rotator = UKismetMathLibrary::RLerp(Monster->GetActorRotation(), Distance.GetSafeNormal().Rotation(),
		CurrentTime, false);
		Monster->SetActorRotation(Rotator);
		if (CurrentTime > 1)
		{
			CurrentTime = 0.0f;
			bRotator = false;
		}
	}
	
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Monster);
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit,Monster->GetActorLocation() + FVector(0,0,300),
		Monster->GetActorLocation() + FVector(0,0,-300),ECC_Visibility,Params);
	if(bHit)
	{
		FRotator Rotator = UKismetMathLibrary::MakeRotFromYZ(Monster->GetActorRightVector(),Hit.ImpactNormal);
		Monster->SetActorRotation(Rotator);
	}
}

void UMonFsm::LineDestination()
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Monster);

	FVector Start = Monster->GetActorLocation() + RandLocation(RandLocationFloat,RandLocationFloat) + FVector(0,0,2000);
	FVector End = Start + FVector(0,0, -10000);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Params);
	if(bHit)
	{
		TargetLocation = Hit.ImpactPoint;
		// DrawDebugLine(GetWorld(),Start,TargetLocation,FColor::Red,false,2.0f);
		// DrawDebugSphere(GetWorld(),TargetLocation,10,10,FColor::Red,false,2.0f);
	}
}

void UMonFsm::MyTimer(TFunction<void()> Func, float time)
{
	bTimer = false;
	CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime > time)
	{
		Func();
		bTimer = true;
		CurrentTime = 0.0f;
	}
}


