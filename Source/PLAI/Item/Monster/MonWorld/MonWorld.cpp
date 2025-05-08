// Fill out your copyright notice in the Description page of Project Settings.


#include "MonWorld.h"

#include "MonWorldFsm.h"


// Sets default values
AMonWorld::AMonWorld()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMonWorld::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMonWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveToLocation();
}

// Called to bind functionality to input
void AMonWorld::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

FVector AMonWorld::RandLocation()
{
	float x = FMath::FRandRange(-350.f,350.f);
	float y = FMath::FRandRange(-350.f,350.f);
	float z = FMath::FRandRange(20.f,100.f);

	FHitResult hit;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(hit,FVector(x,y,z) + FVector(0,0,2000),
		FVector(x,y,z) + FVector(0,0,-2000), ECC_WorldStatic, params);
	DrawDebugSphere(GetWorld(),hit.Location,30,30,FColor::Red);
	return FVector(hit.Location);
}

void AMonWorld::MoveToLocation()
{
	CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime <=  GetWorld()->GetTimeSeconds())
	{
		InitLoc = GetActorLocation() + RandLocation();
	}
	if (FVector::Distance(GetActorLocation(),InitLoc) > 25)
	{
		FVector Dist = InitLoc - GetActorLocation(); 
		AddActorWorldOffset(Dist.GetSafeNormal() * 5);
		SetActorRotation(Dist.Rotation());
	}
	else
	{
		InitLoc = GetActorLocation() + RandLocation();
		CurrentTime = 0;
	}
}


