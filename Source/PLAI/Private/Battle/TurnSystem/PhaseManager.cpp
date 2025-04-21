// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/TurnSystem/PhaseManager.h"

#include "Battle/TurnSystem/TurnManager.h"
#include "Developer/AITestSuite/Public/AITestsCommon.h"
#include "Enemy/BaseEnemy.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BattlePlayer.h"

void AUPhaseManager::BeginPlay()
{
	Super::BeginPlay();

	turnManager = Cast<ATurnManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), turnManagerFactory));

	SetPhase(EBattlePhase::BattleStart);
}

void AUPhaseManager::SetCycle()
{
	cycle = FMath::Min(5, cycle + 1);
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
		InitBattle();
		break;
	case EBattlePhase::RoundStart:
		// 속도를 기반으로 유닛 정렬 작업 및 다음 턴 시작
		break;
	case EBattlePhase::TurnProcessing:
		break;
	case EBattlePhase::RoundEnd:
		// 주기 넘기기
		SetCycle();
		break;
	case EBattlePhase::BattleEnd:
		// 끝났을 때 결과에 대한 UI 보여주면 될듯함.
		break;
	default:
		break;
	}
}

void AUPhaseManager::InitBattle()
{
	// 사이클 시작
	SetCycle();
	// Unit Queue에 세팅
	SetUnitQueue();
	// 배틀 시작
	StartBattle();
}

void AUPhaseManager::SetPlayerAP()
{
	curPlayerAP = maxActionPoints;
	UE_LOG(LogTemp, Warning, TEXT("Player AP Set %d"), curPlayerAP);
}

void AUPhaseManager::SetEnemyAP()
{
	curEnemyAP = maxActionPoints;
	UE_LOG(LogTemp, Warning, TEXT("Enemy AP Set %d"), curEnemyAP);
}

int32 AUPhaseManager::GetAP(int32 ap)
{
	// AP 증가시켜서 리턴
	return ++ap;
}

int32 AUPhaseManager::ConsumeAP(int32 ap, int32 amount)
{
	// 호출하는 대상에 ap를 받아서 AP 증감처리 
	int32 tempAP = FMath::Max(0, ap - amount);
	// 처리된 ap를 다시 호출한 대상 AP에 업데이트
	return tempAP;
}

bool AUPhaseManager::CanUseSkill(int32 ap, int32 cost)
{
	// 호출하는 대상의 AP를 검사해서 크다면 스킬 실행
	return ap >= cost;
}

void AUPhaseManager::SetUnitQueue()
{
	TArray<AActor*> unitArr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), unitFactory, unitArr);

	// 큐 초기화
	unitQueue.Empty();
	// 월드 상에 있는 Actor들을 모아서 유닛 큐에 하나씩 집어 넣기
	for (AActor* unit : unitArr)
	{
		if (ABaseBattlePawn* pawn = Cast<ABaseBattlePawn>(unit))
		{
			unitQueue.Add(pawn);
			UE_LOG(LogTemp, Warning, TEXT("Queue Size %d, Actor Name : %s"),
			       unitQueue.Num(), *pawn->GetActorNameOrLabel());
		}
	}

	// 속도 빠른 순으로 내림차순 정렬
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

void AUPhaseManager::StartBattle()
{
	if (unitQueue.Num() <= 0)
	{
		// 주기 세팅 후 큐 속도에 따라 다시 세팅
		SetCycle();
		SetUnitQueue();
		UE_LOG(LogTemp, Warning, TEXT("unitQueue Empty"));
		return;
	}

	ABaseBattlePawn* tempUnit = unitQueue[0];
	// 현재 턴이 진행 중인 유닛 저장
	turnManager->curUnit = tempUnit;
	// 큐에 들어 있는 처음 유닛 제거
	unitQueue.RemoveAt(0);

	if (auto* curUnit = Cast<ABattlePlayer>(tempUnit))
	{
		// 플레이어 턴 시작
		StartPlayerPhase();
	}
	else
	{
		// 애너미 턴 시작
		StartEnemyPhase();
	}
}

void AUPhaseManager::StartPlayerPhase()
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerTurn Start Unit Name : %s"),
	       *turnManager->curUnit->GetActorNameOrLabel());

	SetPhase(EBattlePhase::TurnProcessing);
	turnManager->curTurnState = ETurnState::PlayerTurn;

	// Player AP 세팅
	SetPlayerAP();
	// PlayerTurn 시작
	turnManager->SetTurnState(ETurnState::PlayerTurn);
}

void AUPhaseManager::EndPlayerPhase()
{
	SetPhase(EBattlePhase::TurnProcessing);
	UE_LOG(LogTemp, Warning, TEXT("End Player Turn"));

	if (unitQueue.Num() > 0)
	{
		turnManager->curUnit = unitQueue[0];
		unitQueue.RemoveAt(0);
	}
	else UE_LOG(LogTemp, Warning, TEXT("End Player Phase unitQueue Empty"));


	// Casting을 통해 현재 유닛이 player 또는 enemy라면 그쪽 함수 실행
	if (auto* player = Cast<ABattlePlayer>(turnManager->curUnit))
	{
		// 시작하기전에 큐 세팅
		SortUnitQueue();
		// PlayerPhase 시작
		StartPlayerPhase();
	}
	else if (auto* enemy = Cast<ABaseEnemy>(turnManager->curUnit))
	{
		// 시작하기전에 큐 세팅
		SortUnitQueue();
		// EnemyPhase 시작
		StartEnemyPhase();
	}
}

void AUPhaseManager::StartEnemyPhase()
{
	SetPhase(EBattlePhase::TurnProcessing);
	turnManager->curTurnState = ETurnState::EnemyTurn;

	UE_LOG(LogTemp, Warning, TEXT("Start Enemy Turn"));

	// EnemyAP 세팅
	SetEnemyAP();

	// 처음 시작할 적 유닛 턴 시작
	if (turnManager)
	{
		turnManager->SetTurnState(ETurnState::EnemyTurn);
		turnManager->StartEnemyTurn();
	}
}

void AUPhaseManager::EndEnemyPhase()
{
	SetCycle();
	if (cycle >= 5)
	{
		SetPhase(EBattlePhase::BattleEnd);
		return;
	}
	if (unitQueue.Num() > 0)
	{
		turnManager->curUnit = unitQueue[0];
		unitQueue.RemoveAt(0);
	}
	else UE_LOG(LogTemp, Warning, TEXT("End Enemy Phase unitQueue Empty"));

	// Casting을 통해 현재 유닛이 player 또는 enemy라면 그쪽 함수 실행
	if (auto* player = Cast<ABattlePlayer>(turnManager->curUnit))
	{
		// PlayerPhase 시작
		StartPlayerPhase();
	}
	else if (auto* enemy = Cast<ABaseEnemy>(turnManager->curUnit))
	{
		// EnemyPhase 시작
		StartEnemyPhase();
	}
}
