// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/Tile/GridTile.h"

#include "Components/BoxComponent.h"

// Sets default values
AGridTile::AGridTile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(boxComp);
	boxComp->SetRelativeScale3D(FVector(1.0f, 1.0f, 0.1f));
	boxComp->SetBoxExtent(FVector(50));
}

// Called when the game starts or when spawned
void AGridTile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGridTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGridTile::SetGridCoord(FIntPoint coord)
{
	gridCoord = coord;
	// UE_LOG(LogTemp, Warning, TEXT("Coord X = %d Coord Y = %d, TileName = %s"),
	//        gridCoord.X, gridCoord.Y, *this->GetActorNameOrLabel());
}
