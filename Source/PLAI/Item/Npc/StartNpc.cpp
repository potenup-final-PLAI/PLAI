// Fill out your copyright notice in the Description page of Project Settings.


#include "StartNpc.h"


// Sets default values
AStartNpc::AStartNpc()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AStartNpc::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AStartNpc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AStartNpc::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

