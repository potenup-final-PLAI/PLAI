// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemOther.h"


// Sets default values
AItemOther::AItemOther()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AItemOther::BeginPlay()
{
	ItemStruct.ItemTop = 2;
	Super::BeginPlay();
	// StaticMesh->SetStaticMesh(ItemParent->ItemMeshStruct.StaticMeshes[0]);
}

// Called every frame
void AItemOther::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

