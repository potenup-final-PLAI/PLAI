// Fill out your copyright notice in the Description page of Project Settings.


#include "PLAI/Public/Battle/Tile/GridTileManager.h"

#include "BaseBattlePawn.h"
#include "GridTile.h"
#include "Algo/RandomShuffle.h"
#include "Battle/UI/ActionUI.h"
#include "Developer/AITestSuite/Public/AITestsCommon.h"
#include "Enemy/BaseEnemy.h"
#include "Evaluation/MovieSceneTimeWarping.h"
#include "Player/BattlePlayer.h"

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
	TArray<FIntPoint> AllCoords;
	// AllCoords.Reserve(625);
	AllCoords.Reserve(49);

	for (int32 Y = 0; Y < 7; ++Y)
	{
		for (int32 X = 0; X < 7; ++X)
		{
			FVector spawnLoc = GetActorLocation() + FVector(
				Y * 100, X * 100, 0.0f);
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
				AllCoords.Add(Coord); // 좌표 저장
			}
		}
	}

	Algo::RandomShuffle(AllCoords);

	TArray<FIntPoint> PlayerCoords;
	for (int32 i = 0; i < 1 && i < AllCoords.Num(); ++i)
	{
		PlayerCoords.Add(AllCoords[i]);
	}

	// 플레이어 좌표를 AllCoords에서 제거
	for (const FIntPoint& Coord : PlayerCoords)
	{
		AllCoords.Remove(Coord);
	}

	// 그 다음 적 좌표 뽑기
	TArray<FIntPoint> EnemyCoords;
	for (int32 i = 0; i < 1 && i < AllCoords.Num(); ++i)
	{
		EnemyCoords.Add(AllCoords[i]);
	}

	// 플레이어 유닛 스폰
	for (const FIntPoint& Coord : PlayerCoords)
	{
		if (AGridTile* Tile = map.FindRef(Coord))
		{
			FVector SpawnLoc = Tile->GetActorLocation() + FVector(
				0.f, 0.f, 10.f);
			auto* player = GetWorld()->SpawnActor<ABattlePlayer>(
				battlePlayerFactory, SpawnLoc, FRotator::ZeroRotator);
			player->speed = FMath::RandRange(1, 10);
		}
	}

	// 적 유닛 스폰
	for (const FIntPoint& Coord : EnemyCoords)
	{
		if (AGridTile* Tile = map.FindRef(Coord))
		{
			FVector SpawnLoc = Tile->GetActorLocation() + FVector(
				0.f, 0.f, 10.f);
			auto* enemey = GetWorld()->SpawnActor<ABaseEnemy>(
				enemyFactory, SpawnLoc,
				FRotator::ZeroRotator);
			enemey->speed = FMath::RandRange(1, 10);
		}
	}
}
AGridTile* AGridTileManager::FindCurrentTile(FVector worldLoc)
{

	FVector loc = worldLoc - GetActorLocation();
	int32 x = FMath::RoundToInt(loc.Y / 100.0f);
	int32 y = FMath::RoundToInt(loc.X / 100.0f);
	
	return map.FindRef(FIntPoint(x, y));
}
