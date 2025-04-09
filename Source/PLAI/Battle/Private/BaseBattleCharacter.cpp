// Fill out your copyright notice in the Description page of Project Settings.


#include "PLAI/Battle/Public/BaseBattleCharacter.h"


// Sets default values
ABaseBattleCharacter::ABaseBattleCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseBattleCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseBattleCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABaseBattleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

