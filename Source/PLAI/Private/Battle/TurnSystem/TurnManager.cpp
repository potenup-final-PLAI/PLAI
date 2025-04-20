// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/TurnSystem/TurnManager.h"

#include "Battle/TurnSystem/PhaseManager.h"


// Sets default values
ATurnManager::ATurnManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATurnManager::BeginPlay()
{
	Super::BeginPlay();
	phaseManager = Cast<AUPhaseManager>(GetWorld()->GetGameState());
}

// Called every frame
void ATurnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATurnManager::SetTurnState(ETurnState newTurnState)
{
	curTurnState = newTurnState;

	switch (curTurnState)
	{
	case ETurnState::None:
		break;
	case ETurnState::PlayerTurn:
		break;
	case ETurnState::EnemyTurn:
		break;
	case ETurnState::TurnEnd:
		break;
	default:
		break;
	}
}

void ATurnManager::SetEnemyTurnList(TArray<ABaseBattlePawn*>& enemies)
{
	enemyQueue = enemies;
}

void ATurnManager::StartEnemyTurn()
{
	if (enemyQueue.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Enemy"));
		return;
	}

	StartNextEnemyTurn();
}

void ATurnManager::StartNextEnemyTurn()
{
	if (enemyQueue.Num() == 0)
	{
		if (phaseManager)
		{
			phaseManager->EndEnemyPhase();
			return;
		}
	}

	curEnemy = enemyQueue[0];
	enemyQueue.RemoveAt(0);

	// FSM 활성화
	if (curEnemy)
	{
		curEnemy->OnTurnStart();
	}
}
