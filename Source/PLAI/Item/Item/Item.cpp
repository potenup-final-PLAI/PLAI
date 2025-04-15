// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "Components/BoxComponent.h"


// Sets default values
AItem::AItem()
{
	BoxComp = CreateDefaultSubobject<UBoxComponent>("BoxComp");
	BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(BoxComp);
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(BoxComp);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	StaticMesh->SetStaticMesh(ItemMeshStructIndexArray[ItemStruct.ItemIndex].
		ItemMeshStructIndex[ItemStruct.ItemIndex].StaticMeshes[RandIndex()]);
}
int32 AItem::RandIndex()
{
	int32 a = ItemStruct.ItemIndex;
	int32 b = ItemStruct.ItemIndex;
	// int32 c = ItemStruct.ItemIndexDetail;
	int32 Index = FMath::RandRange(0,ItemMeshStructIndexArray[a].ItemMeshStructIndex[b].
		StaticMeshes.Num()-1);
	return Index;
}


// Called every frame


