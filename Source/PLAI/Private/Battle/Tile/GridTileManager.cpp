// Fill out your copyright notice in the Description page of Project Settings.


#include "PLAI/Public/Battle/Tile/GridTileManager.h"

#include "GridTile.h"
#include "Evaluation/MovieSceneTimeWarping.h"

// Sets default values
AGridTileManager::AGridTileManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGridTileManager::BeginPlay()
{
	Super::BeginPlay();
	InitGridTile();
}

// Called every frame
void AGridTileManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGridTileManager::InitGridTile()
{
	for (int32 Y = 0; Y < 25; ++Y)
	{
		for (int32 X = 0; X < 25; ++X)
		{
			FVector spawnLoc = GetActorLocation() + FVector(
				Y * 110, X * 110, 0.0f);
			FRotator spawnRot = FRotator::ZeroRotator;
			FActorSpawnParameters spawnParams;

			auto* spawnTile = GetWorld()->SpawnActor<AGridTile>(
				tileFactory, spawnLoc, spawnRot, spawnParams);
			if (spawnTile)
			{
				// X, Y번 째 좌표 기억 변수 생성
				FIntPoint Coord(X, Y);
				// 각 타일에 좌표 기억 시키기 
				spawnTile->SetGridCoord(Coord);
				// 전체적인 타일 관리할 Map에 추가
				map.Add(Coord, spawnTile);
				FString s = map.FindRef(FIntPoint(X, Y))->GetActorNameOrLabel();
				// UE_LOG(LogTemp, Warning, TEXT("X = %d, Y = %d, TileName = %s"),
				//        X, Y, *s);
			}
		}
	}
}
