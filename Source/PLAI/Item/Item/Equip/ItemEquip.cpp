// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemEquip.h"


// Sets default values
AItemEquip::AItemEquip()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
}

// Called when the game starts or when spawned
void AItemEquip::BeginPlay()
{
	ItemStruct.ItemTop = 1;
	Super::BeginPlay();
}

// Called every frame

