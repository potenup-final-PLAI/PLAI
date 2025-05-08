// Fill out your copyright notice in the Description page of Project Settings.


#include "MonGriffon.h"

#include "MonFsmGriffon.h"
#include "PLAI/Item/Monster/MonFsm.h"


// Sets default values
AMonGriffon::AMonGriffon()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MonFsm = CreateDefaultSubobject<UMonFsmGriffon>("MonFsm");
}

// Called when the game starts or when spawned
void AMonGriffon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMonGriffon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMonGriffon::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

