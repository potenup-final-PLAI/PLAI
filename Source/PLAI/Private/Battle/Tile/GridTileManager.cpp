// Fill out your copyright notice in the Description page of Project Settings.


#include "PLAI/Public/Battle/Tile/GridTileManager.h"

#include "BaseBattlePawn.h"
#include "GridTile.h"
#include "Algo/RandomShuffle.h"
#include "Enemy/BaseEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "PLAI/Item/Monster/MonWorld/MonBossPawn.h"
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
	
	TArray<FIntPoint> allCoords;
	allCoords.Reserve(625);
	// allCoords.Reserve(49);

	for (int32 Y = 0; Y < 25; ++Y)
	{
		for (int32 X = 0; X < 25; ++X)
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
				// 타일에 대한 좌표 저장
				tileToCoordMap.Add(spawnTile, Coord);
				FString s = map.FindRef(FIntPoint(X, Y))->GetActorNameOrLabel();
				// UE_LOG(LogTemp, Warning, TEXT("X = %d, Y = %d, TileName = %s"),
				//        X, Y, *s);
				// 좌표 저장
				allCoords.Add(Coord);
			}
		}
	}

	// 좌표 섞기
	Algo::RandomShuffle(allCoords);

	// player 좌표 뽑기
	TArray<FIntPoint> playerCoords;
	for (int32 i = 0; i < 2 && i < allCoords.Num(); ++i)
	{
		playerCoords.Add(allCoords[i]);
	}

	// 플레이어 좌표를 AllCoords에서 제거
	for (const FIntPoint& Coord : playerCoords)
	{
		allCoords.Remove(Coord);
	}
	
	TArray<FIntPoint> enemyCoords;
	
	FString levelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	// Level이 보스 레벨이라면
	if (levelName == "MK_BossMap")
	{
		// 그 다음 적 좌표 뽑기
		for (int32 i = 0; i < 1 && i < allCoords.Num(); ++i)
		{
			enemyCoords.Add(allCoords[i]);
		}
	}
	else
	{
		// 그 다음 적 좌표 뽑기
		for (int32 i = 0; i < 4 && i < allCoords.Num(); ++i)
		{
			enemyCoords.Add(allCoords[i]);
		}
	}
	
	// 플레이어 유닛 스폰
	for (const FIntPoint& coord : playerCoords)
	{
		if (AGridTile* gridTile = map.FindRef(coord))
		{
			FVector spawnLoc = gridTile->GetActorLocation() + FVector(
				0.f, 0.f, 80.f);
			if (auto* player = GetWorld()->SpawnActor<ABattlePlayer>(
				battlePlayerFactory, spawnLoc, FRotator::ZeroRotator))
			{
				// player->speed = FMath::RandRange(1, 10);
				player->currentTile = gridTile;
				unitArray.Add(player);
			}
		}
	}

	// 적 유닛 스폰
	for (const FIntPoint& coord : enemyCoords)
	{
		if (AGridTile* gridTile = map.FindRef(coord))
		{
			// 보스 레벨이라면 보스 몬스터 스폰
			if (levelName == "MK_BossMap")
			{
				FVector spawnLoc = gridTile->GetActorLocation() + FVector(
				0.f, 0.f, 80.f);
				if (auto* enemy = GetWorld()->SpawnActor<AMonBossPawn>(bossFactory, spawnLoc, FRotator::ZeroRotator))
				{
					enemy->speed = FMath::RandRange(1, 10);
					enemy->currentTile = gridTile;
					unitArray.Add(enemy);
				}
			}
			else
			{
				FVector spawnLoc = gridTile->GetActorLocation() + FVector(0.f, 0.f, 80.f);
				if (auto* enemy = GetWorld()->SpawnActor<ABaseEnemy>(enemyFactory, spawnLoc, FRotator::ZeroRotator))
				{
					enemy->speed = FMath::RandRange(1, 10);
					enemy->currentTile = gridTile;
					unitArray.Add(enemy);
				}
			}
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

AGridTile* AGridTileManager::GetTile(int32 x, int32 y)
{
	return map.FindRef(FIntPoint(x, y));
}

FIntPoint AGridTileManager::GetTileLoc(AGridTile* targetTile)
{
	if (targetTile && tileToCoordMap.Contains(targetTile))
	{
		// UE_LOG(LogTemp, Warning, TEXT("%s의 좌표 : (%d, %d)"), *tile->GetActorNameOrLabel(), coord.X, coord.Y);
		return tileToCoordMap[targetTile];
	}

	// 타일이 없다면
	UE_LOG(LogTemp, Warning, TEXT("no have tileToCoordMap[Tile] Coord"));
	return FIntPoint(-1, -1);
}

bool AGridTileManager::IsValidTile(FIntPoint num)
{
	// 좌표를 받아서 그 위치에 타일이 있는지 없는지 체크
	if (map.Contains(num))
	{
		return true;
	}

	return false;
}

void AGridTileManager::SetTileColor(AGridTile* targetTile,  bool bHighlight)
{
	if (!targetTile || !targetTile->dynDecalInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetTileColor : !targetTile || !targetTile->dynDecalInstance"));
		return;
	}

	if (bHighlight)
	{
		//그 위치 타일 색 변경
		targetTile->dynDecalInstance->SetScalarParameterValue(TEXT("TileOpacity"), 0.1f);
	}
	else
	{
		//그 위치 타일 색 변경
		targetTile->dynDecalInstance->SetScalarParameterValue(TEXT("TileOpacity"), 0.0f);
	}
}
