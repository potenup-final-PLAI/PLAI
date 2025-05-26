// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnMonsterWorld.h"

#include "Components/SphereComponent.h"


// Sets default values
ATurnMonsterWorld::ATurnMonsterWorld()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATurnMonsterWorld::BeginPlay()
{
	Super::BeginPlay();
	
	TargetLocation = RandomLoc(500,500);
}

// Called very frame
void ATurnMonsterWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	RandomLocation();
}

// Called to bind functionality to input
void ATurnMonsterWorld::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATurnMonsterWorld::RandomLocation()
{
	AddActorWorldOffset((TargetLocation - GetActorLocation()).GetSafeNormal() * 5);
	if (FVector::Distance(GetActorLocation(),TargetLocation) < 25)
	{
		TargetLocation = RandomLoc(500,500);
		SetActorRotation((TargetLocation - GetActorLocation()).GetSafeNormal().Rotation());
	}
}

FVector ATurnMonsterWorld::RandomLoc(float x, float y)
{
	float X = FMath::FRandRange(-x, x);
	float Y = FMath::FRandRange(-y, y);

	FHitResult hit;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	FCollisionObjectQueryParams objParams;
	objParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);

	bool bHIt = GetWorld()->LineTraceSingleByObjectType(hit,GetActorLocation() + FVector(X,Y,2000),
		GetActorLocation() + FVector(X,Y,-3000),objParams,params);
	if (bHIt)
	{
		// DrawDebugSphere(GetWorld(), hit.Location, 25, 30, FColor::Red,false,2);
		return hit.Location;
	}
	return GetActorLocation();
}



