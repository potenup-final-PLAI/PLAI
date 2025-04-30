// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/BattlePlayer/BaseBattlePawn.h"

#include "BasePlayerState.h"
#include "EnhancedInputSubsystems.h"
#include "GridTile.h"
#include "GridTileManager.h"
#include "Battle/Http/BattleHttpActor.h"
#include "Battle/TurnSystem/PhaseManager.h"
#include "Battle/TurnSystem/TurnManager.h"
#include "Enemy/BaseEnemy.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BattlePlayer.h"

// Sets default values
ABaseBattlePawn::ABaseBattlePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseBattlePawn::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;

	turnManager = Cast<ATurnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), turnManagerFactory));
	
}

// Called every frame
void ABaseBattlePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// if (bIsMoving)
	// {
	// 	UnitMove();
	// }
	if (bIsMoving && pathArray.IsValidIndex(currentPathIndex))
	{
		FVector targetLoc = pathArray[currentPathIndex]->GetActorLocation() + FVector(0, 0, 10);
		FVector currentLoc = GetActorLocation();
		FVector nextLoc = FMath::VInterpConstantTo(currentLoc, targetLoc, DeltaTime, 500.f);
		SetActorLocation(nextLoc);

		if (FVector::DistSquared(currentLoc, targetLoc) < FMath::Square(5.f))
		{
			++currentPathIndex;
			if (!pathArray.IsValidIndex(currentPathIndex))
			{
				bIsMoving = false;
				currentTile = pathArray.Last();
				pathArray.Empty();

				if (auto* enemy = Cast<ABaseEnemy>(this))
				{
					// Player 찾아서 공격
					enemy->FindAndAttackPlayer();
				}
				else
				{
					// 이동 끝났으면 턴 종료 처리
					OnTurnEnd();
				}
			}
		}
	}
}

void ABaseBattlePawn::OnTurnStart()
{
	// 턴이 시작됐으면 턴 카운트 1 증가
	if (turnManager)
	{
		turnManager->turnCount++;
		UE_LOG(LogTemp, Warning, TEXT("Turn Count %d"), turnManager->turnCount);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TurnManager is nullptr"));
	}
	
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(this))
	{
		
		UE_LOG(LogTemp, Warning, TEXT("BaseBattlePawn::OnTurnStart"));
		ABaseBattlePawn* CapturedUnit = this;
		
		FTimerHandle battleAPIHandle;
		GetWorld()->GetTimerManager().SetTimer(battleAPIHandle, FTimerDelegate::CreateLambda([this, CapturedUnit]()
		{
			UE_LOG(LogTemp, Warning, TEXT("BaseBattlePawn::In Lambda"));
			if (turnManager && turnManager->phaseManager)
			{
				UE_LOG(LogTemp, Warning, TEXT("BaseBattlePawn::turnManager, phaseManager is Set"));
				turnManager->phaseManager->TrySendbattleState(CapturedUnit);
			}
		}), 4.0f, false);
	}
	UE_LOG(LogTemp, Warning, TEXT("%s Turn Start"), *GetName());
}
// 자동 탐색 전 가까운 적을 찾는 로직
// AGridTileManager* tileManger = Cast<AGridTileManager>(UGameplayStatics::GetActorOfClass(GetWorld(), TileManagerFactory));
//
// TArray<AActor*> playerActors;
// UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABattlePlayer::StaticClass(), playerActors);
//
// TArray<ABattlePlayer*> players;
// for (AActor* actor : playerActors)
// {
// 	if (ABattlePlayer* p = Cast<ABattlePlayer>(actor))
// 	{
// 		players.Add(p);
// 	}
// }
//
// if (ABattlePlayer* target = enemy->FindClosestPlayer(players))
// {
// 	enemy->MoveToPlayer(target, tileManger);
// }
		
// FTimerHandle timerHandle;
// GetWorld()->GetTimerManager().SetTimer(timerHandle, FTimerDelegate::CreateLambda([this]()
// {
// 	OnTurnEnd();
// }), 5.0f, false);
void ABaseBattlePawn::OnTurnEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("%s Turn End"), *GetName());
	// 입력 막고 FSM 종료
	
	// Casting을 통해 현재 유닛이 player 또는 enemy라면 그쪽 함수 실행
	if (auto* phaseManager = Cast<AUPhaseManager>(GetWorld()->GetGameState()))
	{
		// 턴 종료 상태라면 return
		if (phaseManager->turnManager->curTurnState == ETurnState::TurnEnd) return;
		
		if (ABattlePlayer* player = Cast<ABattlePlayer>(this))
		{
			// PlayerPhaseEnd
			phaseManager->EndPlayerPhase();
		}
		else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(this))
		{
			// EnemyPhaseEnd
			phaseManager->EndEnemyPhase();
		}
	}
}
void ABaseBattlePawn::TestClick()
{
	FVector start, end, dir;
	FHitResult hitInfo;
	FCollisionQueryParams params;

	GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(start, dir);
	end = start + dir * 10000;

	if (GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Visibility, params))
	{
		if (AGridTile* testTile = Cast<AGridTile>(hitInfo.GetActor()))
		{
			float dist = FVector::Dist(GetActorLocation(), testTile->GetActorLocation());
			if (dist <= 700)
			{
				FVector targetLoc = testTile->GetActorLocation();
				SetActorLocation(targetLoc);
				// goalTile 쪽으로 이동
				targetTile = testTile;
				currentTile = testTile;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Test Click : Dist too be far"));
			}
		}
		else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
		{
			BaseAttack(enemy);
		}
		else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
		{
			BaseAttack(player);
		}
	}
}

void ABaseBattlePawn::AddOpenByOffset(FIntPoint offset)
{
	FIntPoint nextCoord = currentTile->gridCoord + offset;
	AGridTileManager* tileManger = Cast<AGridTileManager>(UGameplayStatics::GetActorOfClass(GetWorld(), TileManagerFactory));

	if (AGridTile* tile = tileManger->map.FindRef(nextCoord))
	{
		float previousCost = tile->tCostValue;
		tile->SetCost(currentTile, goalTile);

		if (!tile->parentTile || tile->tCostValue < previousCost)
		{
			tile->parentTile = currentTile;
			
			int32 i = 0;
			for (; i < openArray.Num(); ++i)
			{
				if (openArray[i]->tCostValue >= tile->tCostValue) break;
			}
			openArray.Insert(tile, i);
		}
		// if (!openArray.Contains(tile) && !closedArray.Contains(tile))
		// {
		// 	tile->SetCost(currentTile, goalTile);
		// 	int32 i = 0;
		// 	for (i = 0; i < openArray.Num(); ++i)
		// 	{
		// 		if (openArray[i]->tCostValue >= tile->tCostValue) break;
		// 	}
		// 	openArray.Insert(tile, i);
		// }
	}
}

void ABaseBattlePawn::TryInitStatus()
{
	if (ABasePlayerState* myState = Cast<ABasePlayerState>(GetPlayerState()))
	{
		
		state = myState;
		SetStatus();
		bIsInitialized = true;
		UE_LOG(LogTemp, Warning, TEXT("BaseBattlePawn::TryInitStatus bIsInitialized %d"), bIsInitialized);
		GetWorld()->GetTimerManager().ClearTimer(timerHandle);
	}
}
void ABaseBattlePawn::SetStatus()
{
	UE_LOG(LogTemp, Warning, TEXT("Set state Start"));
	if (ABattlePlayer* player = Cast<ABattlePlayer>(this))
	{
		if (!state)
		{
			UE_LOG(LogTemp, Error, TEXT("SetStatus called but 'state' is null!"));
			return;
		}
		
		UE_LOG(LogTemp, Warning, TEXT("!!! Before setting PlayerStatus"));
		state->playerStatus.hp = 100;
		state->playerStatus.attack = 7;
		state->playerStatus.defense = 10;
		state->playerStatus.resistance = 7;
		state->playerStatus.move_Range = 5;
		state->playerStatus.critical_Rate = 0.05f;
		state->playerStatus.critical_Damage = 1.5f;
		state->playerStatus.speed = 5;

		state->playerLifeState = ELifeState::Alive;
		UE_LOG(LogTemp, Warning, TEXT("!!! After setting PlayerStatus"));
		
		UE_LOG(LogTemp, Warning, TEXT("Player State Set hp : %d, atk : %d, def : %d, res : %d, mov : %d, "
				", crit : %f, crit_Damge : %f, spd : %d"),
				state->playerStatus.hp,
				state->playerStatus.attack,
				state->playerStatus.defense,
				state->playerStatus.resistance,
				state->playerStatus.move_Range,
				state->playerStatus.critical_Rate,
				state->playerStatus.critical_Damage,
				state->playerStatus.speed);
					
		UE_LOG(LogTemp, Warning, TEXT("state : %s"), *UEnum::GetValueAsString(state->playerLifeState));
	}
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(this))
	{
		// 오크 세팅
		UE_LOG(LogTemp, Warning, TEXT("!!! Before setting PlayerStatus"));
		state->enemyStatus.hp = 80;
		state->enemyStatus.attack = 9;
		state->enemyStatus.defense = 6;
		state->enemyStatus.resistance = 3;
		state->enemyStatus.move_Range = 5;
		state->enemyStatus.critical_Rate = 0.04f;
		state->enemyStatus.critical_Damage = 1.5f;
		state->enemyStatus.speed = 4;

		state->enemyLifeState = ELifeState::Alive;
		UE_LOG(LogTemp, Warning, TEXT("!!! After setting PlayerStatus"));
		
		UE_LOG(LogTemp, Warning, TEXT("Enemy State Set hp : %d, atk : %d, def : %d, res : %d, mov : %d, "
				", crit : %f, crit_Damge : %f, spd : %d"),
				state->enemyStatus.hp,
				state->enemyStatus.attack,
				state->enemyStatus.defense,
				state->enemyStatus.resistance,
				state->enemyStatus.move_Range,
				state->enemyStatus.critical_Rate,
				state->enemyStatus.critical_Damage,
				state->enemyStatus.speed);
		
		UE_LOG(LogTemp, Warning, TEXT("state : %s"), *UEnum::GetValueAsString(state->enemyLifeState));

	}
	else{
		UE_LOG(LogTemp, Warning, TEXT("Set state fail"));
	}
}

void ABaseBattlePawn::GetDamage(ABaseBattlePawn* unit, int32 damage)
{
	// player HP가 0보다 크다면
	UE_LOG(LogTemp, Warning, TEXT("In GetDamage"));
	if (ABattlePlayer* player = Cast<ABattlePlayer>(unit))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast ABaseEnemy"));
		if (state && state->playerStatus.hp > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("state && state->playerStatus.hp > 0"));
			// 피를 깎는다.
			if (player && state)
			{
				state->playerStatus.hp = FMath::Max(0, state->playerStatus.hp - damage);
				UE_LOG(LogTemp, Warning, TEXT("Damage : %d, playerHP : %d"), damage, state->playerStatus.hp);
			}
		}
		else
		{
			if (player && state)
			{
				state->playerLifeState = ELifeState::Dead;
				UE_LOG(LogTemp, Warning, TEXT("Enemy Dead %s"), *UEnum::GetValueAsString(state->playerLifeState));
			}
		}
	}
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(unit))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast ABaseEnemy"));
		if (state && state->enemyStatus.hp > 0)
		{
			// 피를 깎는다.
			if (enemy && state)
			{
				state->enemyStatus.hp = FMath::Max(0, state->enemyStatus.hp - damage);
				UE_LOG(LogTemp, Warning, TEXT("Damage : %d, enemyHP : %d"), damage, state->enemyStatus.hp);
			}
		}
		else
		{
			if (enemy && state)
			{
				state->enemyLifeState = ELifeState::Dead;
				UE_LOG(LogTemp, Warning, TEXT("Enemy Dead %s"), *UEnum::GetValueAsString(state->enemyLifeState));
			}
		}
	}
}

void ABaseBattlePawn::BaseAttack(ABaseBattlePawn* targetUnit)
{
	// (기본스탯 + 장비) * 스킬 계수 * (1 + (성격 + 상태효과))
	if (auto* player = Cast<ABattlePlayer>(targetUnit))
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy Attack Player"));
		if (auto* enemy = Cast<ABaseEnemy>(this))
		{
			UE_LOG(LogTemp, Warning, TEXT("In Cast Enemy"));
			
			if (!state)
			{
				UE_LOG(LogTemp, Warning, TEXT("state nullptr"));
				return;
			}
			
			int32 atk = state->enemyStatus.attack;
			int32 weapon = 2;
			float critical = state->enemyStatus.critical_Rate;
			int32 personality = 2;
			int32 status_effect = 4;
			int32 damage = 0;

			// 반올림 처리
			int32 chance = FMath::RoundToInt(critical * 100.0f);
			int32 roll = FMath::RandRange(1, 100);

			
			if (bool bIsCrit = roll <= chance)
			{
				int32 critical_Damage = state->enemyStatus.critical_Damage;
				damage = ((atk + weapon) * 1.0f * (1 + (personality + status_effect))) * critical_Damage;
				UE_LOG(LogTemp, Warning, TEXT("Critical Damage : %d"), damage);
			}
			else
			{
				damage = (atk + weapon) * 1.0f * (1 + (personality + status_effect));
				UE_LOG(LogTemp, Warning, TEXT("Damage : %d"), damage);
			}
			GetDamage(player, damage);
		}
	}
	else if (auto* enemy = Cast<ABaseEnemy>(targetUnit))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Attack Enemy"));
		if (auto* baseAttackPlayer = Cast<ABattlePlayer>(this))
		{
			UE_LOG(LogTemp, Warning, TEXT("In Cast baseAttackPlayer"));
			
			if (!state)
			{
				UE_LOG(LogTemp, Warning, TEXT("state nullptr"));
				return;
			}
			
			int32 atk = state->playerStatus.attack;
			int32 weapon = 2;
			float critical = state->playerStatus.critical_Rate;
			int32 personality = 2;
			int32 status_effect = 4;
			int32 damage = 0;

			// 반올림 처리
			int32 chance = FMath::RoundToInt(critical * 100.0f);
			int32 roll = FMath::RandRange(1, 100);

			
			if (bool bIsCrit = roll <= chance)
			{
				int32 critical_Damage = state->playerStatus.critical_Damage;
				damage = ((atk + weapon) * 1.0f * (1 + (personality + status_effect))) * critical_Damage;
				UE_LOG(LogTemp, Warning, TEXT("Critical Damage : %d"), damage);
			}
			else
			{
				damage = (atk + weapon) * 1.0f * (1 + (personality + status_effect));
				UE_LOG(LogTemp, Warning, TEXT("Damage : %d"), damage);
			}
			GetDamage(enemy, damage);
		}
	}
}


void ABaseBattlePawn::MouseClick(const FInputActionValue& value)
{
	if (!bIsMoveMode) return;
	
	FHitResult hitInfo;
	FVector start, end, dir;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	// 마우스 클릭 위치 얻기
	GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(start, dir);
	end = start + dir * 10000;

	if (GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Visibility, params))
	{
		AGridTile* clickTile = Cast<AGridTile>(hitInfo.GetActor());
		if (clickTile)
		{
			goalTile = clickTile;

			// 시작 타일 다시 설정
			FHitResult startHit;
			FVector pawnStart = GetActorLocation();
			FVector pawnEnd = pawnStart + FVector::DownVector * 1000;

			if (GetWorld()->LineTraceSingleByChannel(startHit, pawnStart, pawnEnd, ECC_Visibility, params))
			{
				// 시작 타일 직접 설정
				startTile = Cast<AGridTile>(startHit.GetActor());
				if (IsValid(startTile))
				{
					// 시작 비용 초기화
					startTile->sCostValue = 0;
					openArray.Add(startTile);
					// 길찾기 시작
					PathFind();
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("StartTile is Invalid Cannot start pathFinding"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Fail to LineTrace downwards to find clickTile"));
			}

			bIsMoveMode = false; // 클릭했으니까 모드 종료
		}
		
	}
}

void ABaseBattlePawn::PathFind()
{
	if (openArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenArray is empty"));
		return;
	}

	int32 safetyCounter = 0;
	const int32 maxSafetyCount = 1000;
	
	while (openArray.Num() > 0 && safetyCounter++ < maxSafetyCount)
	{
		if (safetyCounter > maxSafetyCount)
		{
			UE_LOG(LogTemp, Warning, TEXT("Path Safe Break"));
			break;
		}
		currentTile = openArray[0];
		openArray.RemoveAt(0);

		if (!IsValid(currentTile))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s currentTile PathFind Error"), *GetName());
			return;
		}
		if (!IsValid(goalTile))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s goalTile PathFind Error"), *GetName());
			return;
		}
		// 목표 도달했으면 종료
		if (currentTile == goalTile)
		{
			BuildPath();
			return;
		}

		if (IsValid(currentTile))
		{
			AddOpenByOffset({1, 0});
			AddOpenByOffset({-1, 0});
			AddOpenByOffset({0, 1});
			AddOpenByOffset({0, -1});

			closedArray.Add(currentTile);
			// AddOpenArray(FVector::ForwardVector);
			// AddOpenArray(FVector::BackwardVector);
			// AddOpenArray(FVector::LeftVector);
			// AddOpenArray(FVector::RightVector);

			
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("길을 찾지 못했습니다"));
}

void ABaseBattlePawn::BuildPath()
{
	TSet<AGridTile*> visitePathTiles;
	
	// 찾은 길 표시
	AGridTile* temp = goalTile;
	while (temp && temp->parentTile)
	{
		if (visitePathTiles.Contains(temp))
		{
			UE_LOG(LogTemp, Warning, TEXT(" Infinite loop detected in BuildPath"));
			break;
		}
		visitePathTiles.Add(temp);
		pathArray.Insert(temp, 0); // 역방향으로 삽입
		temp = temp->parentTile;
	}

	// 경로 저장 완료했으면 이동 시작
	if (pathArray.Num() > moveRange)
	{
		UE_LOG(LogTemp, Warning, TEXT("Path too long! Max Move Range = %d, Path Length = %d"), moveRange, pathArray.Num());
        
		// 경로 초기화하고 이동 금지
		pathArray.Empty();
		bIsMoving = false;

		return;
	}
	
	if (pathArray.Num() > 0)
	{
		bIsMoving = true;
		currentPathIndex = 0;
	}
}

void ABaseBattlePawn::AddOpenArray(FVector dir)
{
	FVector start = currentTile->GetActorLocation();
	FVector end = start + dir * 200;
	
	FHitResult hitInfo;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	params.AddIgnoredActor(currentTile);

	DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 5.0f);
	
	if (bool isHit = GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Visibility, params))
	{
		AGridTile* tile = Cast<AGridTile>(hitInfo.GetActor());
		// openArray, closeArray에 해당 tile이 없을 때만
		if (IsValid(tile) && !openArray.Contains(tile) && !closedArray.Contains(tile))
		{
			// 해당 tile에 Cost 구하기
			tile->SetCost(currentTile, goalTile);
			
			// openArray 값에 tile 추가 (cost 값이 작은 tile 앞쪽에)
			int32 i = 0;
			for (i = 0; i < openArray.Num(); ++i)
			{
				if (openArray[i]->tCostValue >= tile->tCostValue)
				{
					break;
				}
			}
			openArray.Insert(tile, i);
		}
	}
}

void ABaseBattlePawn::UnitMove()
{
	// 유닛이 찾은 길을 따라 움직이기
	if (pathArray.IsValidIndex(currentPathIndex))
	{
		FVector targetLoc = pathArray[currentPathIndex]->GetActorLocation();
		FVector currentLoc = GetActorLocation();

		FVector newLoc = FMath::VInterpConstantTo(currentLoc, targetLoc, GetWorld()->GetDeltaSeconds(), moveSpeed);

		SetActorLocation(newLoc);
		
		// 목표 지점에 거의 도착하면 다음 목표로 이동
		if (FVector::DistSquared(newLoc, targetLoc) < FMath::Square(5.0f))
		{
			SetActorLocation(targetLoc);
			++currentPathIndex;

			// 인덱스가 array보다 크거나 같다면
			if (currentPathIndex >= pathArray.Num())
			{
				// 이동 종료
				bIsMoving = false;
				// AI 액션 처리를 위한 함수
				OnMoveEnd();
			}
		}
	}
	else
	{
		bIsMoving = false;
	}
}

void ABaseBattlePawn::OnMoveEnd()
{
	// AI에 턴을 종료하거나 다음 액션 처리 등을 여기서 처리
	UE_LOG(LogTemp, Warning, TEXT("Move Complete"));
}

void ABaseBattlePawn::InitValues()
{
	// 부모 타일 참조 초기화 (중요!)
	AGridTileManager* tileManager = Cast<AGridTileManager>(UGameplayStatics::GetActorOfClass(GetWorld(), TileManagerFactory));
	if (!tileManager) return;
	
	for (auto& pair : tileManager->map)
	{
		if (pair.Value)
		{
			pair.Value->parentTile = nullptr;
			pair.Value->sCostValue = TNumericLimits<float>::Max();
			pair.Value->tCostValue = TNumericLimits<float>::Max();
		}
	}
	for (AGridTile* tile : closedArray)
	{
		if (IsValid(tile)) tile->parentTile = nullptr;
	}
    
	for (AGridTile* tile : openArray)
	{
		if (IsValid(tile)) tile->parentTile = nullptr;
	}
	
	// 배열 및 변수 초기화
	openArray.Empty();
	closedArray.Empty();
	pathArray.Empty();
	currentPathIndex = 0;
	bIsMoving = false;
	currentTile = nullptr;
	startTile = nullptr;
	goalTile = nullptr;
}

