// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcStore.h"


// Sets default values
ANpcStore::ANpcStore()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANpcStore::BeginPlay()
{
	Super::BeginPlay();
	Item = ItemFactory->GetDefaultObject<AItem>();
	LoadNpcStore();
}

// Called every frame
void ANpcStore::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANpcStore::LoadNpcStore()
{
	for (int32 i = 0; i < 12; i++)
	{
		FItemStruct ItemStruct;
		int32 Top = FMath::RandRange(0,2);
		ItemStruct.ItemTop = Top;
		// int32 Index = FMath::RandRange(0,Item->ItemStructTop.ItemMeshTops[Top].ItemMeshIndexes.Num());
		ItemStruct.ItemIndex = 0;
		// int32 Type = FMath::RandRange(0,Item->ItemStructTop.ItemMeshTops[Top].ItemMeshIndexes[Index].ItemMeshTypes.Num());
		ItemStruct.ItemIndexType = 0;
		// int32 Detail = FMath::RandRange(0,Item->ItemStructTop.ItemMeshTops[Top].ItemMeshIndexes[Index].ItemMeshTypes[Type].StaticMeshes.Num());
		ItemStruct.ItemIndexDetail = 0;

		ItemStructsArray.ItemStructs.Add(ItemStruct);
	}
}

// Called to bind functionality to input
void ANpcStore::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

