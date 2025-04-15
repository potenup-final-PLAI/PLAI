// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "Components/BoxComponent.h"


// Sets default values
AItem::AItem()
{
	BoxComp = CreateDefaultSubobject<UBoxComponent>("BoxComp");
	SetRootComponent(BoxComp);
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(BoxComp);
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	SetMesh();
}

void AItem::SetMesh()
{
	Eindex = static_cast<uint8>(EItemIndex);
	
	UE_LOG(LogTemp, Warning, TEXT("Item index is ItemIndex %d StaticMeshIndex %d"),
		EItemIndex, ItemMeshStructs[0].StaticMeshes.Num());
	
	// StaticMesh->SetStaticMesh(ItemMeshStructs[Eindex].StaticMeshes[0]);
}

int32 AItem::RandIndex()
{
	int32 Index = FMath::RandRange(0,ItemMeshStructs[Eindex].StaticMeshes.Num()-1);
	return Index;
}


// Called every frame


