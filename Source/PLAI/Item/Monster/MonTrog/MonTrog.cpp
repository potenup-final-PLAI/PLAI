// Fill out your copyright notice in the Description page of Project Settings.


#include "MonTrog.h"

#include "MonFsmTrog.h"


// Sets default values
AMonTrog::AMonTrog()
{
	MonFsm = CreateDefaultSubobject<UMonFsmTrog>("MonFsmTrog");
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMonTrog::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMonTrog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMonTrog::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

