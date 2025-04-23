// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/TurnSystem/PhaseManager.h"

#include "BasePlayerState.h"
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
		SetCycle();
		// Unit Queue에 세팅
		SetUnitQueue();
		// 전투 시작
		StartBattle();
		break;
	case EBattlePhase::TurnProcessing:
		break;
	case EBattlePhase::RoundEnd:
		// 속도를 기반으로 유닛 정렬 작업 및 다음 턴 시작
		SetCycle();
		// Unit Queue에 세팅
		SetUnitQueue();
		// 전투 시작
		StartBattle();
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
	// 사이클 시작
	SetPhase(EBattlePhase::RoundStart);
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

			// Player State를 가져와서 ! 포제스를 하는 부분에서 해야함.
			// if (auto* ps = Cast<ABasePlayerState>(pawn->GetPlayerState()))
			// {
			// 	// 각 유닛마다 PlayerState 세팅
			// 	ps->SetAP();
			// 	UE_LOG(LogTemp, Warning, TEXT("UnitName : %s, AP : %d"),
			// 	       *pawn->GetActorNameOrLabel(), ps->curAP);
			// }
			// else
			// {
			// 	UE_LOG(LogTemp, Warning, TEXT("Ps Is Fail"));
			// }
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
	// 유닛에 0번을 tempUnit에 저장
	ABaseBattlePawn* tempUnit = unitQueue[0];
	// 현재 턴이 진행 중인 유닛 저장
	turnManager->curUnit = tempUnit;
	// 큐에 들어 있는 처음 유닛 제거
	unitQueue.RemoveAt(0);

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
}

void AUPhaseManager::StartPlayerPhase()
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerTurn Start Unit Name : %s"),
	       *turnManager->curUnit->GetActorNameOrLabel());

	// 턴이 다 지났다면 
	if (unitQueue.Num() <= 0)
	{
		// 주기 세팅 후 큐 속도에 따라 다시 세팅
		
		// UE_LOG(LogTemp, Warning, TEXT("unitQueue Empty"));
	}

	SetPhase(EBattlePhase::TurnProcessing);

	// PlayerTurn 시작
	turnManager->SetTurnState(ETurnState::PlayerTurn);
	// 첫 번째 유닛 포제스
	turnManager->StartPlayerTurn();
}

void AUPhaseManager::EndPlayerPhase()
{
	// 큐에 유닛이 있다면
	if (unitQueue.Num() > 0)
	{
		// 현재 유닛을 0번째에 넣고
		turnManager->curUnit = unitQueue[0];
		// 0번째 유닛 삭제
		unitQueue.RemoveAt(0);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("End Player Phase unitQueue Empty"));
		// 한 주기가 끝났다고 업데이트
		SetPhase(EBattlePhase::RoundEnd);
	}

	// 주기가 5거나 보다 크다면
	if (cycle >= 5)
	{
		// 전투를 끝낸다.
		SetPhase(EBattlePhase::BattleEnd);
		return;
	}
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
	UE_LOG(LogTemp, Warning, TEXT("EnemyTurn Start Unit Name : %s"),
	       *turnManager->curUnit->GetActorNameOrLabel());

	// 턴이 다 지났다면 
	if (unitQueue.Num() <= 0)
	{
		// 주기 세팅 후 큐 속도에 따라 다시 세팅
		SetPhase(EBattlePhase::RoundStart);
		// UE_LOG(LogTemp, Warning, TEXT("unitQueue Empty"));
	}
	SetPhase(EBattlePhase::TurnProcessing);
	turnManager->curTurnState = ETurnState::EnemyTurn;

	UE_LOG(LogTemp, Warning, TEXT("Start Enemy Turn"));

	// 처음 시작할 적 유닛 턴 시작
	if (turnManager)
	{
		turnManager->SetTurnState(ETurnState::EnemyTurn);
		turnManager->StartEnemyTurn();
	}
}

void AUPhaseManager::EndEnemyPhase()
{
	// 유닛이 있다면 처음 유닛을 현재 유닛으로 지정하고 큐에 저장된 유닛을 지운다.
	if (unitQueue.Num() > 0)
	{
		turnManager->curUnit = unitQueue[0];
		unitQueue.RemoveAt(0);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("End Enemy Phase unitQueue Empty"));
		// 한 주기가 끝났다고 업데이트
		SetPhase(EBattlePhase::RoundEnd);
	}
	
	// 주기가 5와 같거나 크다면
	if (cycle >= 5)
	{
		// 전투를 끝낸다.
		SetPhase(EBattlePhase::BattleEnd);
		return;
	}
	
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
}
