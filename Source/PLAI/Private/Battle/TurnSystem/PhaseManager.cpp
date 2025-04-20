// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/TurnSystem/PhaseManager.h"

#include "Battle/TurnSystem/TurnManager.h"
#include "Kismet/GameplayStatics.h"

void AUPhaseManager::BeginPlay()
{
	Super::BeginPlay();

	turnManager = Cast<ATurnManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), turnManagerFactory));
}

void AUPhaseManager::SetCycle()
{
	cycle = FMath::Min(5, cycle + 1);
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

void AUPhaseManager::ConsumeAP(int32 amount)
{
	currentActionPoints = FMath::Max(0, currentActionPoints - amount);
}

bool AUPhaseManager::CanUseSkill(int32 cost)
{
	return currentActionPoints >= cost;
}

void AUPhaseManager::StartPlayerPhase()
{
	SetPhase(EBattlePhase::TurnProcessing);

	turnManager->SetTurnState(ETurnState::PlayerTurn);
}

void AUPhaseManager::EndPlayerPhase()
{
	SetPhase(EBattlePhase::TurnProcessing);
	UE_LOG(LogTemp, Warning, TEXT("End Player Turn"));
	// 턴 종료
	StartEnemyPhase();
}

void AUPhaseManager::StartEnemyPhase()
{
	SetPhase(EBattlePhase::TurnProcessing);

	UE_LOG(LogTemp, Warning, TEXT("Start Enemy Turn"));

	currentActionPoints = maxActionPoints;

	// 적 유닛 목록 세팅

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

	StartPlayerPhase();
}
