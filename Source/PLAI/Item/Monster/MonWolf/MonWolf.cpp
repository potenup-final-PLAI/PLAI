// Fill out your copyright notice in the Description page of Project Settings.


#include "MonWolf.h"

#include "MonFsmWolf.h"


// Sets default values
AMonWolf::AMonWolf()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MonFsm = CreateDefaultSubobject<UMonFsmWolf>("MonFsm");
}

// Called when the game starts or when spawned
void AMonWolf::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMonWolf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMonWolf::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

