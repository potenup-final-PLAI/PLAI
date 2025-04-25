// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemConsume.h"


// Sets default values
AItemConsume::AItemConsume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AItemConsume::BeginPlay()
{
	ItemStruct.ItemTop = 0;
	Super::BeginPlay();
}

// Called every frame
void AItemConsume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

