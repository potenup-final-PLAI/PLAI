// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature.h"

#include "CreFsm.h"
#include "Dragon/CreDragon.h"


// Sets default values
ACreature::ACreature()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACreature::BeginPlay()
{
	Super::BeginPlay();

	CreatureParent = CreatureFactory->GetDefaultObject<ACreature>();
}

// Called every frame
void ACreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACreature::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

