// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnMonster.h"

#include "Components/SphereComponent.h"


// Sets default values
ATurnMonster::ATurnMonster()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATurnMonster::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATurnMonster::Tick(float DeltaTime)
{
	ACharacter::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATurnMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

