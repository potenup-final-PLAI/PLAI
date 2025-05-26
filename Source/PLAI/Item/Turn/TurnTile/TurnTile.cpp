﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnTile.h"

#include "Misc/MapErrors.h"


// Sets default values
ATurnTile::ATurnTile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	SetRootComponent(StaticMeshComp);
}

// Called when the game starts or when spawned
void ATurnTile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATurnTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

