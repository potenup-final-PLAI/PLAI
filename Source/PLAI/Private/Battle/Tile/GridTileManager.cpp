// Fill out your copyright notice in the Description page of Project Settings.


#include "PLAI/Public/Battle/Tile/GridTileManager.h"

#include "BaseBattlePawn.h"
#include "GridTile.h"
#include "Algo/RandomShuffle.h"
#include "Battle/TurnSystem/BattlePlayerController.h"
#include "Battle/TurnSystem/PhaseManager.h"
#include "Battle/Util/DebugHeader.h"
#include "Enemy/BaseEnemy.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PLAI/Item/GameInstance/WorldGi.h"
#include "PLAI/Item/Monster/MonWorld/MonBossPawn.h"
#include "Player/BattlePlayer.h"

// Sets default values
AGridTileManager::AGridTileManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

// Called when the game starts or when spawned
void AGridTileManager::BeginPlay()
{
	Super::BeginPlay();
}

void AGridTileManager::GetLifetimeReplicatedProps(
	TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGridTileManager, tile);
	DOREPLIFETIME(AGridTileManager, tileFactory);
}

// Called every frame
void AGridTileManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AGridTileManager::ServerRPC_InitGridTile_Implementation()
{
	TArray<FIntPoint> allCoords;
	allCoords.Reserve(625);

	// Tile 스폰 작업
	for (int32 Y = 0; Y < 25; ++Y)
	{
		for (int32 X = 0; X < 25; ++X)
		{
			FVector spawnLoc = GetActorLocation() + FVector(Y * 100, X * 100, 0.0f);
			FRotator spawnRot = FRotator::ZeroRotator;
			FActorSpawnParameters spawnParams;
			
			auto* spawnTile = GetWorld()->SpawnActor<AGridTile>(tileFactory, spawnLoc, spawnRot, spawnParams);
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
				
				SendTileData(Coord, spawnTile);
				// 좌표 저장
				allCoords.Add(Coord);
			}
		}
	}

	// 좌표 섞기
	Algo::RandomShuffle(allCoords);

	// PlayerController 수만큼 좌표 확보
	TArray<APlayerState*> playerStates = GetWorld()->GetGameState()->PlayerArray;
	UE_LOG(LogTemp, Warning, TEXT("playerStates : %d"), playerStates.Num());
	
	TArray<FIntPoint> playerCoords = RandomCoords(playerStates.Num(), allCoords);
	
	for (int32 i = 0; i < playerStates.Num(); ++i)
	{
		const FIntPoint& coord = playerCoords[i];
		AGridTile* gridTile = map.FindRef(coord);
		if (!gridTile)
		{
			continue;
		}

		// 유닛 스폰
		FVector spawnLoc = gridTile->GetActorLocation() + FVector(0.f, 0.f, 80.f);
		ABattlePlayer* player = GetWorld()->SpawnActor<ABattlePlayer>(battlePlayerFactory, spawnLoc, FRotator::ZeroRotator);
		if (!player)
		{
			continue;
		}

		player->currentTile = gridTile;
		APlayerState* ps = playerStates[i];
		ABattlePlayerController* battlePC = Cast<ABattlePlayerController>(ps->GetPlayerController());
		if (battlePC)
		{
			if (player->GetOwner() != nullptr) UE_LOG(LogTemp, Warning, TEXT("Before player Owner %s, Player : %s"), *player->GetOwner()->GetActorNameOrLabel(), *player->GetActorNameOrLabel());
			player->SetOwner(battlePC);
			if (HasAuthority())  battlePC->Possess(player);
			player->MultiCastRPC_SetMyName(i);
			if (player->GetOwner() != nullptr) UE_LOG(LogTemp, Warning, TEXT("After player Owner %s, Player : %s"), *player->GetOwner()->GetActorNameOrLabel(), *player->GetActorNameOrLabel());
		
			player->ForceNetUpdate(); // 복제 보장
			playerControllers.Add(battlePC);
		}
		
		unitArray.Add(player);
		allCoords.Remove(coord);
	}
	// Enemy 스폰 작업
	TArray<FIntPoint> enemyCoords;

	// 어떤 레벨인제 체크해서 분기
	FString levelName = UGameplayStatics::GetCurrentLevelName(GetWorld());

	// Level이 보스 레벨이라면
	if (levelName == "MK_BossMap")
	{
		// 그 다음 적 좌표 뽑기
		enemyCoords.Add(allCoords[0]);
	}
	else
	{
		// 그 다음 적 좌표 뽑기
		for (int32 i = 0; i < 4; ++i)
		{
			enemyCoords.Add(allCoords[i]);
		}
	}

	int32 enemyCount = 0;
	
	// 적 유닛 스폰
	for (const FIntPoint& coord : enemyCoords)
	{
		if (AGridTile* gridTile = map.FindRef(coord))
		{
			// 보스 레벨이라면 보스 몬스터 스폰
			if (levelName == "MK_BossMap")
			{
				FVector spawnLoc = gridTile->GetActorLocation() + FVector(0.f, 0.f, 80.f);
				// Enemy 스폰
				if (auto* enemy = GetWorld()->SpawnActor<AMonBossPawn>(bossFactory, spawnLoc, FRotator::ZeroRotator))
				{
					// enemy 스피드 설정
					enemy->speed = FMath::RandRange(1, 10);
					// 현재 타일 설정
					enemy->currentTile = gridTile;
					// Array에 추가
					unitArray.Add(enemy);
				}
			}
			else
			{
				FVector spawnLoc = gridTile->GetActorLocation() + FVector(0.f, 0.f, 80.f);
				if (auto* enemy = GetWorld()->SpawnActor<ABaseEnemy>(enemyFactory, spawnLoc, FRotator::ZeroRotator))
				{
					UE_LOG(LogTemp, Warning, TEXT("Spawning enemy %s at tile coord (%d, %d), tile = %s, tile location = %s"), *enemy->GetName(), coord.X, coord.Y, *gridTile->GetName(), *gridTile->GetActorLocation().ToString());
					// 위 내용과 동일
					enemy->speed = FMath::RandRange(1, 10);
					enemy->currentTile = gridTile;
					enemy->MyName = TEXT("Unit_") + FString::FormatAsNumber(enemyCount++);
					unitArray.Add(enemy);
				}
			}
		}
	}
	if (auto* phaseManager = Cast<AUPhaseManager>(GetWorld()->GetGameState()))
	{
		phaseManager->SetBeforeBattle();
	}
	// UE_LOG(LogTemp, Warning, TEXT("InitGridTile : gridTile Complete"));


	// 타일 Array만들어서 변수 담기
	TArray<FTileNetData> netTileDataArray;
	for (const TPair<FIntPoint, AGridTile*>& pair : map)
	{
		FTileNetData tileData;
		tileData.tileName = pair.Value->GetName(); // 고유 이름으로 식별
		tileData.coord = pair.Key;
		tileData.tileInstance = map.FindRef(pair.Key);
		netTileDataArray.Add(tileData);
	}
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, netTileDataArray]()
	{
		MulticastRPC_InitClientMap(netTileDataArray);
	}, 1.5f, false); // 1초 후에 호출
}

void AGridTileManager::SendTileData_Implementation(FIntPoint coord, AGridTile* tilePointer)
{
	// 전체적인 타일 관리할 Map에 추가
	map.Add(coord, tilePointer);
	// 타일에 대한 좌표 저장
	tileToCoordMap.Add(tilePointer, coord);
}

TArray<FIntPoint> AGridTileManager::RandomCoords(int32 count, TArray<FIntPoint> coords)
{
	TArray<FIntPoint> result;

	// allCoords는 전체 타일 좌표 리스트
	TArray<FIntPoint> shuffledCoords = coords;
	shuffledCoords.Sort([](const FIntPoint& A, const FIntPoint& B)
	{
		return FMath::RandBool(); // 랜덤 셔플
	});

	for (int32 i = 0; i < count && i < shuffledCoords.Num(); ++i)
	{
		result.Add(shuffledCoords[i]);
	}

	return result;
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
	auto key = FIntPoint(x, y);

	if (map.Contains(key))
		return map.FindRef(key);

	return nullptr;
}


FIntPoint AGridTileManager::GetTileLoc(AGridTile* targetTile)
{
	// if (tileToCoordMap.Contains(targetTile))
	// {
	// 	NET_PRINTLOG(TEXT("tileToCoordMap.Contains(targetTile)"))	
	// }
	// else if (targetTile != nullptr)
	// {
	// 	NET_PRINTLOG(TEXT("TargetTile : %s"), *targetTile->GetActorNameOrLabel());
	// }
	// else
	// {
	// 	NET_PRINTLOG(TEXT("no have TargetTile"));
	// }
	// if (tileToCoordMap.Num() <= 0)
	// {
	// 	NET_PRINTLOG(TEXT("tileToCoordMap.Num() <= 0"));
	// }
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

void AGridTileManager::SetTileColor(AGridTile* targetTile, bool bHighlight)
{
	if (!targetTile || !targetTile->dynDecalInstance)
	{
		UE_LOG(LogTemp, Warning,TEXT("SetTileColor : !targetTile || !targetTile->dynDecalInstance"));
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

void AGridTileManager::MulticastRPC_InitClientMap_Implementation(const TArray<FTileNetData>& tileData)
{
	if (HasAuthority()) return; // 서버는 패스
	
	for (const FTileNetData& data : tileData)
	{
		if (AGridTile* tempTile = data.tileInstance)
		{
			map.Add(data.coord, tempTile);
			tileToCoordMap.Add(tempTile, data.coord);
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("클라이언트 타일 맵 초기화 완료: %d개"), map.Num());
}
