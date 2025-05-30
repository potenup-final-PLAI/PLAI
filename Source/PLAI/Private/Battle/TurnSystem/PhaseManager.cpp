// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/TurnSystem/PhaseManager.h"

#include "GridTile.h"
#include "GridTileManager.h"
#include "Battle/Http/BattleHttpActor.h"
#include "Battle/TurnSystem/BattlePlayerController.h"
#include "Battle/TurnSystem/TurnManager.h"
#include "Battle/UI/BattleHUD.h"
#include "Battle/UI/CycleAndTurn.h"
#include "Battle/UI/MainBattleUI.h"
#include "Battle/UI/WorldDamageUIActor.h"
#include "Battle/Util/DebugHeader.h"
#include "Enemy/BaseEnemy.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BattlePlayer.h"
#include "Battle/Util/BattleType/BattleTypes.h"
#include "Net/UnrealNetwork.h"
#include "PLAI/Item/GameInstance/WorldGi.h"

void AUPhaseManager::BeginPlay()
{
	Super::BeginPlay();

	InitOtherClass();

	gridTileManager = Cast<AGridTileManager>(UGameplayStatics::GetActorOfClass(GetWorld(), girdTileManagerFactory));
	if (gridTileManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid tile manager created"));
	}
	if (pc)
	{
		this->SetOwner(pc);
	}
	// GetWorld()->GetTimerManager().SetTimer(timerBattleStartHandle, this,&AUPhaseManager::SetBeforeBattle,0.2f, true);
}

void AUPhaseManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AUPhaseManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUPhaseManager, cycle);
	DOREPLIFETIME(AUPhaseManager, currentPhase);
	DOREPLIFETIME(AUPhaseManager, unitQueue);
	DOREPLIFETIME(AUPhaseManager, turnManager);
	DOREPLIFETIME(AUPhaseManager, turnOrderArray);
}

void AUPhaseManager::SetCycle()
{
	cycle = FMath::Min(6, cycle + 1);
	UE_LOG(LogTemp, Warning, TEXT("Cur Cycle = %d"), cycle);
	if (HasAuthority())
	{
		UpdateCycleUI();
	}
}

void AUPhaseManager::UpdateCycleUI()
{
	if (!pc || !hud || !hud->mainUI)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT(
			       "AUPhaseManager::UpdateCycleUI : !pc || !hud || !hud->mainUI"
		       ));
		return;
	}

	hud->mainUI->WBP_CycleAndTurn->SetCycleText(cycle);
}

void AUPhaseManager::OnRep_UpdateCycleUI()
{
	UpdateCycleUI();
}

void AUPhaseManager::OnRep_CurrentPhase()
{
	// UI 상태 변경과 같은 클라이언트 처리가 필요할 때 사용
	UE_LOG(LogTemp, Warning, TEXT("Phase changed to: %s"),*UEnum::GetValueAsString(currentPhase));
}

void AUPhaseManager::SetPhase(EBattlePhase phase)
{
	currentPhase = phase;

	NET_PRINTLOG(TEXT("AUPhaseManager::SetPhase currentPhase : %s"), *UEnum::GetValueAsString(phase));

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
		TurnPorcessing(turnManager->curUnit);
		break;
	case EBattlePhase::RoundEnd:
		// 턴 카운트 초기화
		turnManager->turnCount = 0;
		UE_LOG(LogTemp, Warning,TEXT("PhaseManager : RoundEnd State Turn CNT : %d"),turnManager->turnCount);
		SetPhase(EBattlePhase::RoundStart);
		break;
	case EBattlePhase::BattleEnd:
		// 끝났을 때 결과에 대한 UI 보여주면 될듯함.
		//BattleEnd();
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
	unitQueue.Empty();
	httpUnitQueue.Empty();

	int32 alivePlayers = 0;
	int32 aliveEnemies = 0;

	for (AActor* unit : gridTileManager->unitArray)
	{
		if (ABaseBattlePawn* pawn = Cast<ABaseBattlePawn>(unit))
		{
			if (ABattlePlayer* player = Cast<ABattlePlayer>(pawn))
			{
				if (player->hp <= 0)
				{
					continue;
				}
				alivePlayers++;
			}
			else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(pawn))
			{
				if (!enemy || enemy->hp <=0)
				{
					continue;
				}
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
		if (alivePlayers == 0)
		{
			NET_PRINTLOG(TEXT("BattleEnd reason : alivePlayers == 0"));
		}
		else
		{
			NET_PRINTLOG(TEXT("BattleEnd reason : aliveEnemies == 0"));
		}
		SetPhase(EBattlePhase::BattleEnd);
		return;
	}

	SortUnitQueue();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		if (turnManager)
		{
			turnManager->Multicast_AddOrderUnit();
			NET_PRINTLOG(TEXT("Delayed Multicast_AddOrderUnit 호출"));
		}
	}, 0.5f, false);
	
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
		if (A->speed == B->speed) return true;
		return A->speed > B->speed;
	});
}

void AUPhaseManager::SortUnitTurnEnd()
{
	for (auto* pawn : unitQueue)
	{
		if (pawn->hp <= 0)
		{
			unitQueue.Remove(pawn);
			httpUnitQueue.Remove(pawn);
			UE_LOG(LogTemp, Warning,TEXT("Removed dead unit from future queue: %s"),*pawn->MyName);
		}
	}
}

ABaseBattlePawn* AUPhaseManager::PopNextAliveUnit()
{
	if (unitQueue.Num() > 0)
	{
		for (int i = 0; i < unitQueue.Num(); i++)
		{
			NET_PRINTLOG(TEXT("UnitName %s Unit Speed : %d"), *unitQueue[i]->MyName, unitQueue[i]->speed);
		}
		
		ABaseBattlePawn* candidate = unitQueue[0];
		if (candidate->hp > 0)
		{
			unitQueue.RemoveAt(0);
			return candidate;
		}
	}
	
	NET_PRINTLOG(TEXT("unitQueue.Num() <= 0"));

	return nullptr;
}


void AUPhaseManager::StartBattle()
{
	DoingPopNextAliveUnit();
	
	NET_PRINTLOG(TEXT("AUPhaseManager::StartBattle"));
}

void AUPhaseManager::TurnPorcessing(ABaseBattlePawn* unit)
{
	if (cycle > 5)
	{
		// 전투를 끝낸다.
		NET_PRINTLOG(TEXT("BattleEnd reason : cycle > 5"));
		SetPhase(EBattlePhase::BattleEnd);
		return;
	}
	turnManager->StartTurn();
	
	// if (ABattlePlayer* player = Cast<ABattlePlayer>(turnManager->curUnit))
	// {
	// 	// 플레이어 턴 시작
	// 	StartPlayerPhase();
	// }
	// else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(turnManager->curUnit))
	// {
	// 	// 애너미 턴 시작
	// 	StartEnemyPhase();
	// }
}

void AUPhaseManager::StartPlayerPhase()
{
	if (!HasAuthority()) return;
	UE_LOG(LogTemp, Warning, TEXT("PhaseManager : Start PlayerPhase"));
	UE_LOG(LogTemp, Warning, TEXT("PlayerTurn Start Unit Name : %s"),*turnManager->curUnit->GetActorNameOrLabel());

	// 주기가 5거나 보다 크다면
	if (cycle > 5)
	{
		// 전투를 끝낸다.
		NET_PRINTLOG(TEXT("BattleEnd reason : cycle > 5"));
		SetPhase(EBattlePhase::BattleEnd);
		return;
	}

	// PlayerTurn 시작
	// 첫 번째 유닛 포제스
	turnManager->StartTurn();
}

void AUPhaseManager::ServerRPC_EndPlayerPhase_Implementation()
{
	if (turnManager->curTurnState == ETurnState::TurnEnd)
	{
		return;
	}
	// 턴 종료 State로 변경
	turnManager->SetTurnState(ETurnState::TurnEnd);

	// 큐에 유닛이 있다면
	DoingPopNextAliveUnit();
	UE_LOG(LogTemp, Warning, TEXT("End Player Phase"));
}

// void AUPhaseManager::StartEnemyPhase()
// {
// 	if (!HasAuthority()) return;
// 	UE_LOG(LogTemp, Warning, TEXT("Start Enemy Turn"));
// 	UE_LOG(LogTemp, Warning, TEXT("EnemyTurn Start Unit Name : %s"),*turnManager->curUnit->GetActorNameOrLabel());
//
// 	// 주기가 5와 같거나 크다면
// 	if (cycle > 5)
// 	{
// 		// 전투를 끝낸다.
// 		NET_PRINTLOG(TEXT("BattleEnd reason : cycle > 5"));
// 		SetPhase(EBattlePhase::BattleEnd);
// 		return;
// 	}
//
// 	// 처음 시작할 적 유닛 턴 시작
// 	if (turnManager)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("PhaseManager in turnManager Set"));
// 		turnManager->StartTurn();
// 	}
// }

void AUPhaseManager::ServerRPC_EndEnemyPhase_Implementation()
{
	if (turnManager->curTurnState == ETurnState::TurnEnd)
	{
		return;
	}

	// 턴 종료 State로 변경
	turnManager->SetTurnState(ETurnState::TurnEnd);

	UE_LOG(LogTemp, Warning, TEXT("PhaseManager : EndEnemyPhase"));

	// 유닛이 있다면 처음 유닛을 현재 유닛으로 지정하고 큐에 저장된 유닛을 지운다.
	DoingPopNextAliveUnit();

	UE_LOG(LogTemp, Warning, TEXT("%s : End Enemy Phase"), *turnManager->curUnit->MyName);
}

void AUPhaseManager::BattleEnd()
{
	turnManager->SetTurnState(ETurnState::None);
	UE_LOG(LogTemp, Warning, TEXT("End Battle"));
	if (HasAuthority())
	{
		GetWorld()->ServerTravel(TEXT("/Game/Mk_Item/Mk_WorldPartition?listen"));
	}
}

void AUPhaseManager::SetBeforeBattle()
{
	// 서버가 아니면 return
	if (!HasAuthority())
	{
		return;
	}

	// unitArray turnOrderArray에 세팅
	turnOrderArray = gridTileManager->unitArray;
	
	// girdTile에 저장해둔 unit들 순서대로 Possess해서 PlayerState 세팅
	if (gridTileManager->unitArray.Num() > 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(timerBattleStartHandle);
		
		TArray<ABaseBattlePawn*> baseUnits = gridTileManager->unitArray;

		for (auto& unit : baseUnits)
		{
			if (!pc)
			{
				continue;
			}
			
			// 유닛 스텟 세팅
			TryInitStatus(unit);

			// 가장 가까운 적 찾기
			ABaseBattlePawn* closestTarget = nullptr;
			float minDistSquared = TNumericLimits<float>::Max();

			for (ABaseBattlePawn* other : baseUnits)
			{
				if (unit == other)
				{
					continue;
				}

				// 같은 팀이면 무시
				const bool bothPlayer = unit->IsA<ABattlePlayer>() && other->IsA<ABattlePlayer>();
				const bool bothEnemy = unit->IsA<ABaseEnemy>() && other->IsA<ABaseEnemy>();

				if (bothPlayer || bothEnemy)
				{
					continue;
				}

				float distSq = FVector::DistSquared(unit->GetActorLocation(), other->GetActorLocation());
				if (distSq < minDistSquared)
				{
					minDistSquared = distSq;
					closestTarget = other;
				}
			}
			if (closestTarget)
			{
				// 기본 회전 보정값
				float yawOffset = -90.f;

				// 매쉬만 회전
				if (ABattlePlayer* player = Cast<ABattlePlayer>(unit))
				{
					if (player->meshComp)
					{
						FVector dir = closestTarget->GetActorLocation() - player->GetActorLocation();
						FRotator meshRot = dir.Rotation();
						player->meshComp->SetRelativeRotation(FRotator(0, meshRot.Yaw + yawOffset, 0));
					}
				}
				else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(unit))
				{
					if (enemy->meshComp)
					{
						FVector dir = closestTarget->GetActorLocation() - enemy->GetActorLocation();
						FRotator meshRot = dir.Rotation();
						enemy->meshComp->SetWorldRotation(FRotator(0, meshRot.Yaw + yawOffset, 0));
					}
				}
				else
				{
					// enemy 임시로 회전
					FVector dir = closestTarget->GetActorLocation() - unit->GetActorLocation();
					FRotator lookRot = dir.Rotation();
					unit->SetActorRotation(FRotator(0, lookRot.Yaw, 0));
				}
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

		if (auto* httpActor = Cast<ABattleHttpActor>(UGameplayStatics::GetActorOfClass(GetWorld(), httpActorFactory)))
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
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ABaseBattlePawn::StaticClass(),unitArr);

	int32 i = 0;
	for (AActor* actor : unitArr)
	{
		UE_LOG(LogTemp, Warning, TEXT("for문 In !!! "));
		if (ABaseBattlePawn* unit = Cast<ABaseBattlePawn>(actor))
		{
			
			UE_LOG(LogTemp, Warning, TEXT("unit is ABaseBattlePawn !!! "));

			UE_LOG(LogTemp, Warning, TEXT("Set API Data !!!"));
			FCharacterData charData;
			charData.id =  unit->GetName();
			FString name = unit->MyName;
			charData.name = Cast<ABattlePlayer>(unit) ? name : TEXT("엔트");
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
				for (int8 j = 0; j < player->traits.Num(); ++j)
				{
					charData.traits.Add(player->traits[j]);
				}
			}
			else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(unit))
			{
				if (!enemy)
				{
					UE_LOG(LogTemp, Error, TEXT("enemy is null: %s"), *enemy->GetName());
					continue;
				}

				// skills 접근 전에 반드시 확인
				if (enemy->skills.Num() <= 0)
				{
					UE_LOG(LogTemp, Error, TEXT("enemy->skills가 비어있거나 enemySkills보다 작습니다. 유닛: %s"), *enemy->GetName());
					continue;
				}

				for (int8 j = 0; j < enemy->skills.Num(); ++j)
				{
					charData.skills.Add(enemy->skills[j]);
				}
				for (int8 j = 0; j < enemy->traits.Num(); ++j)
				{
					charData.traits.Add(enemy->traits[j]);
				}
			}

			env.characters.Add(charData);
			UE_LOG(LogTemp, Warning, TEXT("char id : %s name : %s, type : %s"),*charData.id, *charData.name, *charData.type);
			UE_LOG(LogTemp, Warning, TEXT("Traits : %s, Skills : %s"),*FString::Join(charData.traits, TEXT(", ")),*FString::Join(charData.skills, TEXT(", ")));
		}
		++i;
	}

	// 맵 지형/날씨는 수동 세팅하거나 타일매니저에서 가져올 수 있음
	env.terrain = TEXT("desert");
	env.weather = TEXT("sunny");

	UE_LOG(LogTemp, Warning, TEXT("terrain : %s, weather : %s"), *env.terrain,*env.weather);
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

	// // Queue 점검용 테스트 코드 
	// for (ABaseBattlePawn* unit : httpUnitQueue)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Checking unit: %s"), *unit->GetName());
	//
	// 	if (!IsValid(unit))
	// 	{
	// 		UE_LOG(LogTemp, Error, TEXT("Invalid unit!"));
	// 		continue;
	// 	}
	//
	// 	if (!unit->currentTile)
	// 	{
	// 		UE_LOG(LogTemp, Error, TEXT("Unit %s has no current tile"),*unit->GetName());
	// 		continue;
	// 	}
	//
	// 	// 여기에 도달한 경우만 추가됨
	// 	UE_LOG(LogTemp, Warning, TEXT("✅ Unit %s passed all checks"),*unit->GetName());
	// }
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
		charStatus.position = {unit->currentTile->gridCoord.X, unit->currentTile->gridCoord.Y};

		if (ABattlePlayer* player = Cast<ABattlePlayer>(unit))
		{
			charStatus.hp = player->hp;
			charStatus.ap = player->curAP;
			charStatus.mov = player->moveRange;
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
			charStatus.hp = enemy->hp;
			charStatus.ap = enemy->curAP;
			charStatus.mov = enemy->moveRange;
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
	if (unit->hp <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy HP is 0"));
		return;
	}
	
	if (AreAllUnitsInitialized())
	{
		FBattleTurnState battleData = SetBattleProcessingAPI();

		if (auto* httpActor = Cast<ABattleHttpActor>(UGameplayStatics::GetActorOfClass(GetWorld(), httpActorFactory)))
		{
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
			UE_LOG(LogTemp, Error,TEXT("Unit %s is nullptr"),*unit->GetName());
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
		SetStatus(player);
		
	}
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(unit))
	{
		SetStatus(enemy);
	}
}

void AUPhaseManager::SetStatus(ABaseBattlePawn* unit)
{
	if (ABattlePlayer* player = Cast<ABattlePlayer>(unit))
	{
		// 데이터 받아와서 세팅
		// 장비 장착 
		if (gi && gi->UserFullInfoGiStat.character_info.stats.speed != 0)
		{
			gi->EquipActor(player);

			// 스테이터스 세팅
			player->hp = gi->UserFullInfoGiStat.character_info.stats.hp;
			player->attack = gi->UserFullInfoGiStat.character_info.stats.attack * 2;
			player->defense = gi->UserFullInfoGiStat.character_info.stats.defense;
			player->resistance = gi->UserFullInfoGiStat.character_info.stats.resistance;
			player->moveRange = gi->UserFullInfoGiStat.character_info.stats.move_range * 3;
			player->critical_Rate = gi->UserFullInfoGiStat.character_info.stats.critical_rate;
			player->critical_Damage = gi->UserFullInfoGiStat.character_info.stats.critical_damage;
			player->speed = gi->UserFullInfoGiStat.character_info.stats.speed;
			// 최대 HP 설정
			player->maxHP = gi->UserFullInfoGiStat.character_info.stats.hp;

			for (const FString trait : gi->UserFullInfoGiStat.character_info.traits)
			{
				player->traits.Add(trait);
			}
		}
		else
		{
			player->hp = 100;
			player->attack = 7;
			player->defense = 10;
			player->resistance = 7;
			player->moveRange = 5;
			player->critical_Rate = 0.05f;
			player->critical_Damage = 1.5f;
			player->speed = 10;
			// 최대 HP 설정
			player->maxHP = player->hp;
		}
		
		player->playerLifeState = ELifeState::Alive;
		
		// UE_LOG(LogTemp, Warning,TEXT("Player State Set hp : %d, atk : %d, def : %d, res : %d, mov : %d, crit : %f, crit_Damge : %f, spd : %d"),
		//        player->hp,
		//        player->attack,
		//        player->defense,
		//        player->resistance,
		//        player->moveRange,
		//        player->critical_Rate,
		//        player->critical_Damage,
		//        player->speed);

		// UE_LOG(LogTemp, Warning, TEXT("state : %s"),*UEnum::GetValueAsString(player->playerLifeState));
	}
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(unit))
	{
		enemy->InitEnemyState();
		
		enemy->enemyLifeState = ELifeState::Alive;

		// UE_LOG(LogTemp, Warning,TEXT("PhaseManger SetStatus : Enemy State Set hp : %d, atk : %d, def : %d, res : %d, mov : %d, crit : %f, crit_Damge : %f, spd : %d"),
		//        enemy->hp,
		//        enemy->attack,
		//        enemy->defense,
		//        enemy->resistance,
		//        enemy->moveRange,
		//        enemy->critical_Rate,
		//        enemy->critical_Damage,
		//        enemy->speed);
		//
		// UE_LOG(LogTemp, Warning, TEXT("state : %s"),*UEnum::GetValueAsString(enemy->enemyLifeState));
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

void AUPhaseManager::InitOtherClass()
{
	// GI
	gi = Cast<UWorldGi>(GetWorld()->GetGameInstance());
	if (!gi)
	{
		UE_LOG(LogTemp, Warning, TEXT("AUPhaseManager::InitOtherClass : !gi"));
	}

	// TurnManager
	turnManager = Cast<ATurnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), turnManagerFactory));
	if (!turnManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("AUPhaseManager::InitOtherClass : !turnManager"));
	}

	// PC
	pc = GetWorld()->GetFirstPlayerController();
	if (!pc)
	{
		UE_LOG(LogTemp, Warning, TEXT("AUPhaseManager::InitOtherClass : !pc"));
	}
	else
	{
		// hud 포인터 세팅
		hud = Cast<ABattleHUD>(pc->GetHUD());
		if (!hud) UE_LOG(LogTemp, Warning, TEXT("AUPhaseManager::InitOtherClass : !hud"));
	}

	// girdTileManager
	gridTileManager = Cast<AGridTileManager>(UGameplayStatics::GetActorOfClass(GetWorld(), girdTileManagerFactory));
	if (!gridTileManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("AUPhaseManager::InitOtherClass : !gridTileManager"));
	}

	// DamageUI Spawn이 클라만큼 됨
	damageUIActor = GetWorld()->SpawnActor<AWorldDamageUIActor>(damageUIFactory);
	if (!damageUIActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("AUPhaseManager::InitOtherClass : !damageUIActor"));
	}
}

void AUPhaseManager::PlayerReady(APlayerController* playerControl)
{
	readyCount += 1;
	// UE_LOG(LogTemp, Warning, TEXT("PlayerArray Num : %d readyCount : %d"),GetWorld()->GetGameState()->PlayerArray.Num(), readyCount);
	// if (GetWorld()->GetGameState()->PlayerArray.Num() <= readyCount)
	if (readyCount >= GetWorld()->GetGameState()->PlayerArray.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("모든 플레이어 준비 완료 → 전투 시작"));

		for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it; ++it)
		{
			if (ABattlePlayerController* battlePC = Cast<ABattlePlayerController>(*it))
			{
				battlePC->Client_StopReadyTimer();
			}
		}
		gridTileManager->ServerRPC_InitGridTile();
	}
}

void AUPhaseManager::DoingPopNextAliveUnit()
{
	SortUnitTurnEnd();
	
	ABaseBattlePawn* tempUnit = PopNextAliveUnit();
	if (nullptr == tempUnit)
	{
		NET_PRINTLOG(TEXT("BattleEnd reason : PopNextAliveUnit is nullptr"));
		// 만약 모든 애들의 체력을 확인해서 끝났으면
		int32 playercount = 0;
		int32 enemycount = 0;
		for (auto* unit : gridTileManager->unitArray)
		{
			if (auto* player = Cast<ABattlePlayer>(unit))
			{
				if (player->hp > 0)
					playercount++;
			}
			else
			{
				if (unit->hp > 0)
					enemycount++;
			}			
		}
		turnManager->curUnit = nullptr;
		Multicast_PopNextAliveUnit(tempUnit);
		if (playercount == 0 || enemycount == 0)
			SetPhase(EBattlePhase::BattleEnd); // 또는 RoundEnd
		// 그렇지 않으면
		else
			SetPhase(EBattlePhase::RoundEnd); // 또는 RoundEnd
		return;
	}
	
	turnManager->curUnit = tempUnit;

	SetPhase(EBattlePhase::TurnProcessing);

	Multicast_PopNextAliveUnit(tempUnit);
}


void AUPhaseManager::Multicast_PopNextAliveUnit_Implementation(ABaseBattlePawn* nextCurUnit)
{
	turnManager->curUnit = nextCurUnit;
}

void AUPhaseManager::ClearNetLog()
{
	NetLog = "";
	if (auto* httpActor = Cast<ABattleHttpActor>(UGameplayStatics::GetActorOfClass(GetWorld(), httpActorFactory)))
	{
		httpActor->UpdateDebugLog(NetLog);
	}
}

void AUPhaseManager::AddNetLog(FString msg)
{
	NetLog += msg + "\n";
	if (auto* httpActor = Cast<ABattleHttpActor>(UGameplayStatics::GetActorOfClass(GetWorld(), httpActorFactory)))
	{
		httpActor->UpdateDebugLog(NetLog);
	}
}
