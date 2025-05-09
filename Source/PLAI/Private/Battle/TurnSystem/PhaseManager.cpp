// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/TurnSystem/PhaseManager.h"

#include "BasePlayerState.h"
#include "GridTile.h"
#include "GridTileManager.h"
#include "Battle/Http/BattleHttpActor.h"
#include "Battle/TurnSystem/TurnManager.h"
#include "Battle/UI/BattleHUD.h"
#include "Battle/UI/CycleAndTurn.h"
#include "Battle/UI/MainBattleUI.h"
#include "Enemy/BaseEnemy.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BattlePlayer.h"
#include "Battle/Util/BattleType/BattleTypes.h"
#include "Blueprint/UserWidget.h"
#include "Player/BattlePlayerState.h"

void AUPhaseManager::BeginPlay()
{
	Super::BeginPlay();

	turnManager = Cast<ATurnManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), turnManagerFactory));

	gridTileManager = Cast<AGridTileManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), girdTileManagerFactory));

	GetWorld()->GetTimerManager().SetTimer(timerBattleStartHandle, this,
	                                       &AUPhaseManager::SetBeforeBattle,
	                                       0.2f, true);
}

void AUPhaseManager::SetCycle()
{
	cycle = FMath::Min(6, cycle + 1);
	if (APlayerController* pc = GetWorld()->GetFirstPlayerController())
	{
		if (ABattleHUD* hud = Cast<ABattleHUD>(pc->GetHUD()))
		{
			if (hud->mainUI && hud->mainUI->WBP_CycleAndTurn)
			{
				hud->mainUI->WBP_CycleAndTurn->SetCycleText(cycle);
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Cur Cycle = %d"), cycle);
}

void AUPhaseManager::SetPhase(EBattlePhase phase)
{
	currentPhase = phase;

	switch (currentPhase)
	{
	case EBattlePhase::None:
		break;
	case EBattlePhase::BattleStart:
		// 전투에 필요한 초기화 작업 InitBattle();로 정의
		UE_LOG(LogTemp, Warning, TEXT("PhaseManager : BattleStart State"));
		InitBattle();
		break;
	case EBattlePhase::RoundStart:
		// 속도를 기반으로 유닛 정렬 작업 및 다음 턴 시작
		UE_LOG(LogTemp, Warning, TEXT("PhaseManager : RoundStart State"));
		SetCycle();
		// Unit Queue에 세팅
		SetUnitQueue();
		// 전투 시작
		StartBattle();
		break;
	case EBattlePhase::TurnProcessing:
		break;
	case EBattlePhase::RoundEnd:
		// 턴 카운트 초기화
		turnManager->turnCount = 0;
		UE_LOG(LogTemp, Warning,
		       TEXT("PhaseManager : RoundEnd State Turn CNT : %d"),
		       turnManager->turnCount);
		SetPhase(EBattlePhase::RoundStart);
		break;
	case EBattlePhase::BattleEnd:
		// 끝났을 때 결과에 대한 UI 보여주면 될듯함.
		BattleEnd();
		break;
	default:
		break;
	}
}

void AUPhaseManager::InitBattle()
{
	UE_LOG(LogTemp, Warning, TEXT("PhaseManager : InitBattle"));
	// 사이클 시작
	SetPhase(EBattlePhase::RoundStart);
}

void AUPhaseManager::SetUnitQueue()
{
	TArray<AActor*> unitArr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), unitFactory, unitArr);

	unitQueue.Empty();
	httpUnitQueue.Empty();

	int32 alivePlayers = 0;
	int32 aliveEnemies = 0;

	for (AActor* unit : unitArr)
	{
		if (ABaseBattlePawn* pawn = Cast<ABaseBattlePawn>(unit))
		{
			if (ABattlePlayer* player = Cast<ABattlePlayer>(pawn))
			{
				if (!player->battlePlayerState || player->battlePlayerState->playerStatus.hp <= 0)
					continue;

				player->speed = player->battlePlayerState->playerStatus.speed;
				alivePlayers++;
			}
			else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(pawn))
			{
				if (!enemy->enemybattleState || enemy->enemybattleState->enemyStatus.hp <= 0)
					continue;

				enemy->speed = enemy->enemybattleState->enemyStatus.speed;
				aliveEnemies++;
			}

			unitQueue.Add(pawn);
			httpUnitQueue.Add(pawn);
			UE_LOG(LogTemp, Warning, TEXT("Queue Add: %s"), *pawn->GetName());
		}
	}

	// 전멸 조건 처리
	if (alivePlayers == 0 || aliveEnemies == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("One side has no alive units — Ending battle"));
		SetPhase(EBattlePhase::BattleEnd);
		return;
	}

	SortUnitQueue();
	
	for (int i = 0; i < unitQueue.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnitName %s Unit Speed : %d"),
		       *unitQueue[i]->GetActorNameOrLabel(), unitQueue[i]->speed);
	}
}

void AUPhaseManager::SortUnitQueue()
{
	Algo::Sort(unitQueue, [](ABaseBattlePawn* A, const ABaseBattlePawn* B)
	{
		return A->speed > B->speed;
	});
}

ABaseBattlePawn* AUPhaseManager::PopNextAliveUnit()
{
	while (unitQueue.Num() > 0)
	{
		ABaseBattlePawn* candidate = unitQueue[0];
		unitQueue.RemoveAt(0);

		if (ABattlePlayer* player = Cast<ABattlePlayer>(candidate))
		{
			if (player->battlePlayerState && player->battlePlayerState->playerStatus.hp > 0)
				return candidate;
		}
		else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(candidate))
		{
			if (enemy->enemybattleState && enemy->enemybattleState->enemyStatus.hp > 0)
				return candidate;
		}
	}
	return nullptr;
}


void AUPhaseManager::StartBattle()
{
	SetUnitQueue();

	ABaseBattlePawn* tempUnit = PopNextAliveUnit();
	if (!tempUnit)
	{
		SetPhase(EBattlePhase::BattleEnd); // 또는 RoundEnd
		return;
	}
	turnManager->curUnit = tempUnit;
	
	if (ABattlePlayer* playerUnit = Cast<ABattlePlayer>(tempUnit))
	{
		// 플레이어 턴 시작
		StartPlayerPhase();
	}
	else if (ABaseEnemy* enemyUnit = Cast<ABaseEnemy>(tempUnit))
	{
		// 애너미 턴 시작
		StartEnemyPhase();
	}
	// UE_LOG(LogTemp, Warning, TEXT("PhaseManager : Start Battle"));
	//
	// // 유닛에 0번을 tempUnit에 저장
	// ABaseBattlePawn* tempUnit = unitQueue[0];
	// // 현재 턴이 진행 중인 유닛 저장
	// turnManager->curUnit = tempUnit;
	// // 큐에 들어 있는 처음 유닛 제거
	// unitQueue.RemoveAt(0);
	//
	// if (ABattlePlayer* playerUnit = Cast<ABattlePlayer>(tempUnit))
	// {
	// 	// 플레이어 턴 시작
	// 	StartPlayerPhase();
	// }
	// else if (ABaseEnemy* enemyUnit = Cast<ABaseEnemy>(tempUnit))
	// {
	// 	// 애너미 턴 시작
	// 	StartEnemyPhase();
	// }
}

void AUPhaseManager::StartPlayerPhase()
{
	UE_LOG(LogTemp, Warning, TEXT("PhaseManager : Start PlayerPhase"));
	UE_LOG(LogTemp, Warning, TEXT("PlayerTurn Start Unit Name : %s"),
	       *turnManager->curUnit->GetActorNameOrLabel());

	// 주기가 5거나 보다 크다면
	if (cycle > 5)
	{
		// 전투를 끝낸다.
		SetPhase(EBattlePhase::BattleEnd);
		return;
	}

	SetPhase(EBattlePhase::TurnProcessing);

	// PlayerTurn 시작
	// turnManager->SetTurnState(ETurnState::PlayerTurn);
	// 첫 번째 유닛 포제스
	turnManager->StartPlayerTurn();
}

void AUPhaseManager::EndPlayerPhase()
{
	if (turnManager->curTurnState == ETurnState::TurnEnd)
	{
		return;
	}
	// 턴 종료 State로 변경
	turnManager->SetTurnState(ETurnState::TurnEnd);

	// 큐에 유닛이 있다면
	ABaseBattlePawn* tempUnit = PopNextAliveUnit();
	if (!tempUnit)
	{
		// 유닛이 큐에 없다면 Round 종료
		SetPhase(EBattlePhase::RoundEnd);
		return;
	}
	turnManager->curUnit = tempUnit;
	
	UE_LOG(LogTemp, Warning, TEXT("End Player Phase"));

	SetPhase(EBattlePhase::TurnProcessing);

	// Casting을 통해 현재 유닛이 player 또는 enemy라면 그쪽 함수 실행
	if (ABattlePlayer* player = Cast<ABattlePlayer>(turnManager->curUnit))
	{
		// PlayerPhase 시작
		StartPlayerPhase();
	}
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(turnManager->curUnit))
	{
		// EnemyPhase 시작
		StartEnemyPhase();
	}
}

void AUPhaseManager::StartEnemyPhase()
{
	UE_LOG(LogTemp, Warning, TEXT("PhaseManager : Start Enemy Phase"));
	UE_LOG(LogTemp, Warning, TEXT("EnemyTurn Start Unit Name : %s"),
	       *turnManager->curUnit->GetActorNameOrLabel());

	// 주기가 5와 같거나 크다면
	if (cycle > 5)
	{
		// 전투를 끝낸다.
		SetPhase(EBattlePhase::BattleEnd);
		return;
	}

	SetPhase(EBattlePhase::TurnProcessing);
	// turnManager->curTurnState = ETurnState::EnemyTurn;

	UE_LOG(LogTemp, Warning, TEXT("Start Enemy Turn"));

	// 처음 시작할 적 유닛 턴 시작
	if (turnManager)
	{
		turnManager->StartEnemyTurn();
	}
}

void AUPhaseManager::EndEnemyPhase()
{
	if (turnManager->curTurnState == ETurnState::TurnEnd)
	{
		return;
	}

	// 턴 종료 State로 변경
	turnManager->SetTurnState(ETurnState::TurnEnd);

	UE_LOG(LogTemp, Warning, TEXT("PhaseManager : EndEnemyPhase"));
	
	// 유닛이 있다면 처음 유닛을 현재 유닛으로 지정하고 큐에 저장된 유닛을 지운다.
	ABaseBattlePawn* nextUnit = PopNextAliveUnit();
	if (!nextUnit)
	{
		SetPhase(EBattlePhase::RoundEnd);
		return;
	}
	turnManager->curUnit = nextUnit;


	SetPhase(EBattlePhase::TurnProcessing);
	UE_LOG(LogTemp, Warning, TEXT("End Enemy Phase"));

	// Casting을 통해 현재 유닛이 player 또는 enemy라면 그쪽 함수 실행
	if (ABattlePlayer* player = Cast<ABattlePlayer>(turnManager->curUnit))
	{
		// PlayerPhase 시작
		StartPlayerPhase();
	}
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(turnManager->curUnit))
	{
		// EnemyPhase 시작
		StartEnemyPhase();
	}
}

void AUPhaseManager::BattleEnd()
{
	turnManager->SetTurnState(ETurnState::None);
	UE_LOG(LogTemp, Warning, TEXT("End Battle"));
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Mk_LevelVillage"));
}

void AUPhaseManager::SetBeforeBattle()
{
	// girdTile에 저장해둔 unit들 순서대로 Possess해서 PlayerState 세팅

	if (gridTileManager->unitArray.Num() > 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(timerBattleStartHandle);

		UE_LOG(LogTemp, Warning, TEXT("Set Before Battle "));
		TArray<ABaseBattlePawn*> baseUnits = gridTileManager->unitArray;

		for (auto& unit : baseUnits)
		{
			if (auto* pc = GetWorld()->GetFirstPlayerController())
			{
				pc->Possess(unit);
				// 유닛 스텟 세팅
				TryInitStatus(unit);
				UE_LOG(LogTemp, Warning,
				       TEXT("Set Before Battle : Possess!!!"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Fail PlayerController"));
			}
		}
		// 유닛이 다 세팅되면 API 전송
		TrySendInitialState();
	}
}

void AUPhaseManager::TrySendInitialState()
{
	if (AreAllUnitsInitialized())
	{
		FEnvironmentState envData = SetStartBattleAPI();

		if (auto* httpActor = Cast<ABattleHttpActor>(
			UGameplayStatics::GetActorOfClass(GetWorld(), httpActorFactory)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Start Battle API"));
			httpActor->HttpPost(envData);
		}
	}
}

FEnvironmentState AUPhaseManager::SetStartBattleAPI()
{
	UE_LOG(LogTemp, Warning, TEXT("SSetStartBattleAPI In !!!"));
	FEnvironmentState env;

	TArray<AActor*> unitArr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),
	                                      ABaseBattlePawn::StaticClass(),
	                                      unitArr);

	int32 i = 0;
	for (AActor* actor : unitArr)
	{
		UE_LOG(LogTemp, Warning, TEXT("for문 In !!! "));
		if (ABaseBattlePawn* unit = Cast<ABaseBattlePawn>(actor))
		{
			UE_LOG(LogTemp, Warning, TEXT("unit is ABaseBattlePawn !!! "));
			// if (!unit->battlePlayerState || !unit->enemybattleState) continue;// PlayerState 아직 안 됐으면 건너뜀


			UE_LOG(LogTemp, Warning, TEXT("Set API Data !!!"));
			FCharacterData charData;
			charData.id = unit->GetName();
			FString name = unit->GetName();
			//Cast<ABattlePlayer>(unit) ? FString::Printf(TEXT("player%d"), i) : FString::Printf(TEXT("monster%d"), i);
			//unit->Rename(*name);
			charData.name = Cast<ABattlePlayer>(unit) ? name : TEXT("오크");
			// 따로 이름 필드 있으면
			charData.type = Cast<ABattlePlayer>(unit)
				                ? TEXT("player")
				                : TEXT("monster");

			// 스킬 charData에 넣어서 보낼 준비
			if (ABattlePlayer* player = Cast<ABattlePlayer>(unit))
			{
				for (int8 j = 0; j < player->playerSkills.Num(); ++j)
				{
					charData.skills.Add(player->playerSkills[j]);
				}
			}
			else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(unit))
			{
				for (int8 j = 0; j < enemy->enemySkills.Num(); ++j)
				{
					charData.skills.Add(enemy->enemySkills[j]);
				}
			}
			charData.traits.Add(TEXT("호전적"));

			env.characters.Add(charData);
			UE_LOG(LogTemp, Warning, TEXT("char id : %s name : %s, type : %s"),
			       *charData.id, *charData.name, *charData.type);
			UE_LOG(LogTemp, Warning, TEXT("Traits : %s, Skills : %s"),
			       *FString::Join(charData.traits, TEXT(", ")),
			       *FString::Join(charData.skills, TEXT(", ")));
		}
		++i;
	}

	// 맵 지형/날씨는 수동 세팅하거나 타일매니저에서 가져올 수 있음
	env.terrain = TEXT("desert");
	env.weather = TEXT("sunny");

	UE_LOG(LogTemp, Warning, TEXT("terrain : %s, weather : %s"), *env.terrain,
	       *env.weather);
	return env;
}

FBattleTurnState AUPhaseManager::SetBattleProcessingAPI()
{
	// 1. 구조체 인스턴스 준비
	FBattleTurnState turnStateData;
	// 현재 사이클
	turnStateData.cycle = cycle;
	// 턴 수
	turnStateData.turn = turnManager->turnCount;
	// 유닛에 정의된 ID
	turnStateData.current_character_id = turnManager->curUnit->GetName();

	// 2. 현재 존재하는 모든 유닛 정보 담기
	TArray<ABaseBattlePawn*> AllUnits = httpUnitQueue;

	// Queue 점검용 테스트 코드 
	for (ABaseBattlePawn* unit : httpUnitQueue)
	{
		UE_LOG(LogTemp, Warning, TEXT("Checking unit: %s"), *unit->GetName());

		if (!IsValid(unit))
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid unit!"));
			continue;
		}

		if (!unit->currentTile)
		{
			UE_LOG(LogTemp, Error, TEXT("Unit %s has no current tile"),
			       *unit->GetName());
			continue;
		}

		// 여기에 도달한 경우만 추가됨
		UE_LOG(LogTemp, Warning, TEXT("✅ Unit %s passed all checks"),
		       *unit->GetName());
	}
	for (ABaseBattlePawn* unit : AllUnits)
	{
		if (!IsValid(unit) || !unit->currentTile)
		{
			continue;
		}

		FCharacterStatus charStatus;
		// ID
		charStatus.id = unit->GetName();
		// 타일에 있는 유닛의 위치 
		charStatus.position = {
			unit->currentTile->gridCoord.X, unit->currentTile->gridCoord.Y
		};

		if (ABattlePlayer* player = Cast<ABattlePlayer>(unit))
		{
			charStatus.hp = player->battlePlayerState->playerStatus.hp;
			charStatus.ap = player->curAP;
			charStatus.mov = player->battlePlayerState->playerStatus.move_Range;
			// 상태 이상 정보 추가
			for (const auto& Elem : player->activeStatusEffects)
			{
				EStatusEffect Effect = Elem.Key;
				// 한글 이름으로 변환 후 charStatus에 추가
				charStatus.status_effects.Add(GetStatusEffectsString(Effect));
			}
		}
		else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(unit))
		{
			charStatus.hp = enemy->enemybattleState->enemyStatus.hp;
			charStatus.ap = enemy->enemybattleState->curAP;
			charStatus.mov = enemy->enemybattleState->enemyStatus.move_Range;
			// 상태 이상 정보 추가
			for (const auto& Elem : enemy->activeStatusEffects)
			{
				EStatusEffect Effect = Elem.Key;
				// 한글 이름으로 변환 후 charStatus에 추가
				charStatus.status_effects.Add(GetStatusEffectsString(Effect));
			}
		}

		turnStateData.characters.Add(charStatus);
	}

	return turnStateData;
}

void AUPhaseManager::TrySendbattleState(ABaseBattlePawn* unit)
{
	UE_LOG(LogTemp, Warning, TEXT("TrySendBattleState In "));
	if (AreAllUnitsInitialized())
	{
		UE_LOG(LogTemp, Warning, TEXT("AreAllUnitsInitizlized clear!! "));

		FBattleTurnState battleData = SetBattleProcessingAPI();

		if (auto* httpActor = Cast<ABattleHttpActor>(
			UGameplayStatics::GetActorOfClass(GetWorld(), httpActorFactory)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Battle Action API"));
			httpActor->HttpPost({}, battleData, unit);
		}
	}
}

bool AUPhaseManager::AreAllUnitsInitialized() const
{
	for (ABaseBattlePawn* unit : httpUnitQueue)
	{
		if (!IsValid(unit))
		{
			return false;
		}

		const bool bPlayerValid = Cast<ABattlePlayer>(unit) && unit->
			battlePlayerState != nullptr;
		const bool bEnemyValid = Cast<ABaseEnemy>(unit) && unit->
			enemybattleState != nullptr;

		if (!(bPlayerValid || bEnemyValid))
		{
			UE_LOG(LogTemp, Error,
			       TEXT("Unit %s is missing its player/enemy state"),
			       *unit->GetName());
			return false;
		}
	}
	return true;
}

//--------------------State Setting---------------------------------
void AUPhaseManager::TryInitStatus(ABaseBattlePawn* unit)
{
	if (ABattlePlayer* player = Cast<ABattlePlayer>(unit))
	{
		if (ABattlePlayerState* myState = Cast<ABattlePlayerState>(
			player->GetPlayerState()))
		{
			player->battlePlayerState = myState;
			SetStatus(player);
			bIsInitialized = true;
			UE_LOG(LogTemp, Warning,
			       TEXT("BaseBattlePawn::TryInitStatus bIsInitialized %d"),
			       bIsInitialized);
		}
	}
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(unit))
	{
		SetStatus(enemy);
		bIsInitialized = true;
		UE_LOG(LogTemp, Warning,
		       TEXT("BaseBattlePawn::TryInitStatus bIsInitialized %d"),
		       bIsInitialized);
	}
}

void AUPhaseManager::SetStatus(ABaseBattlePawn* unit)
{
	UE_LOG(LogTemp, Warning, TEXT("Set state Start"));
	if (ABattlePlayer* player = Cast<ABattlePlayer>(unit))
	{
		if (!player->battlePlayerState)
		{
			UE_LOG(LogTemp, Error,
			       TEXT("SetStatus called but 'state' is null!"));
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("!!! Before setting PlayerStatus"));
		player->battlePlayerState->playerStatus.hp = 100;
		player->battlePlayerState->playerStatus.attack = 7;
		player->battlePlayerState->playerStatus.defense = 10;
		player->battlePlayerState->playerStatus.resistance = 7;
		player->battlePlayerState->playerStatus.move_Range = 5;
		player->battlePlayerState->playerStatus.critical_Rate = 0.05f;
		player->battlePlayerState->playerStatus.critical_Damage = 1.5f;
		player->battlePlayerState->playerStatus.speed = FMath::RandRange(1, 10);

		// moveRange 세팅 작업 진행
		player->moveRange = player->battlePlayerState->playerStatus.move_Range;

		player->battlePlayerState->playerLifeState = ELifeState::Alive;
		UE_LOG(LogTemp, Warning, TEXT("!!! After setting PlayerStatus"));

		UE_LOG(LogTemp, Warning,
		       TEXT(
			       "Player State Set hp : %d, atk : %d, def : %d, res : %d, mov : %d, "
			       ", crit : %f, crit_Damge : %f, spd : %d"),
		       player->battlePlayerState->playerStatus.hp,
		       player->battlePlayerState->playerStatus.attack,
		       player->battlePlayerState->playerStatus.defense,
		       player->battlePlayerState->playerStatus.resistance,
		       player->battlePlayerState->playerStatus.move_Range,
		       player->battlePlayerState->playerStatus.critical_Rate,
		       player->battlePlayerState->playerStatus.critical_Damage,
		       player->battlePlayerState->playerStatus.speed);

		UE_LOG(LogTemp, Warning, TEXT("state : %s"),
		       *UEnum::GetValueAsString(player->battlePlayerState->
			       playerLifeState));
	}
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(unit))
	{
		// 오크 세팅
		UE_LOG(LogTemp, Warning, TEXT("!!! Before setting PlayerStatus"));
		enemy->InitEnemyState();

		enemy->enemybattleState->enemyLifeState = ELifeState::Alive;
		UE_LOG(LogTemp, Warning, TEXT("!!! After setting PlayerStatus"));

		UE_LOG(LogTemp, Warning,
		       TEXT(
			       "Enemy State Set hp : %d, atk : %d, def : %d, res : %d, mov : %d, "
			       ", crit : %f, crit_Damge : %f, spd : %d"),
		       enemy->enemybattleState->enemyStatus.hp,
		       enemy->enemybattleState->enemyStatus.attack,
		       enemy->enemybattleState->enemyStatus.defense,
		       enemy->enemybattleState->enemyStatus.resistance,
		       enemy->enemybattleState->enemyStatus.move_Range,
		       enemy->enemybattleState->enemyStatus.critical_Rate,
		       enemy->enemybattleState->enemyStatus.critical_Damage,
		       enemy->enemybattleState->enemyStatus.speed);

		UE_LOG(LogTemp, Warning, TEXT("state : %s"),
		       *UEnum::GetValueAsString(enemy->enemybattleState->enemyLifeState
		       ));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Set state fail"));
	}
}

FString AUPhaseManager::GetStatusEffectsString(EStatusEffect effect)
{
	static const TMap<EStatusEffect, FString> StatusEffectNames = {
		{EStatusEffect::Poison, TEXT("중독")},
		{EStatusEffect::Vulnerable, TEXT("취약")},
		{EStatusEffect::Weakening, TEXT("약화")},
		{EStatusEffect::Angry, TEXT("분노")},
		{EStatusEffect::Bleeding, TEXT("출혈")}
	};

	const FString* FoundName = StatusEffectNames.Find(effect);
	return FoundName ? *FoundName : TEXT("알 수 없음");
}
