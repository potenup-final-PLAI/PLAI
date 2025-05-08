// Fill out your copyright notice in the Description page of Project Settings.


#include "MonWorld.h"

#include "MonWorldFsm.h"


// Sets default values
AMonWorld::AMonWorld()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MonFsm = CreateDefaultSubobject<UMonWorldFsm>("MonWorldFsm");
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
}

// Called to bind functionality to input
void AMonWorld::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

