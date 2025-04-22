// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/TurnSystem/TurnManager.h"

#include "BaseBattlePawn.h"
#include "Battle/TurnSystem/PhaseManager.h"
#include "Developer/AITestSuite/Public/AITestsCommon.h"
#include "Enemy/BaseEnemy.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BattlePlayer.h"

DEFINE_LOG_CATEGORY(TPS);

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

	FString s = UEnum::GetValueAsString(curTurnState);
	// PRINTLOGTOSCREEN(TEXT("%s"), *s);
}

void ATurnManager::SetTurnState(ETurnState newTurnState)
{
	curTurnState = newTurnState;

	switch (curTurnState)
	{
	case ETurnState::None:
		break;
	case ETurnState::PlayerTurn:
		// 플레이어 턴으로 변경
		curTurnState = ETurnState::PlayerTurn;
	// 유닛 포제스
	// Player AP 세팅
		break;
	case ETurnState::EnemyTurn:
		break;
	case ETurnState::TurnEnd:
		break;
	default:
		break;
	}
}

void ATurnManager::StartPlayerTurn()
{
	if (auto* playerPawn = Cast<ABattlePlayer>(curUnit))
	{
		UE_LOG(LogTemp, Display, TEXT("playerPawn is Player %s"),
		       *playerPawn->GetActorNameOrLabel());

		if (APlayerController* pc = GetWorld()->GetFirstPlayerController())
		{
			pc->Possess(playerPawn);
			UE_LOG(LogTemp, Warning, TEXT("possess unit %s"), *playerPawn->GetActorNameOrLabel());
		}

		playerPawn->OnTurnStart();
	}
}

void ATurnManager::StartNextPlayerTurn()
{
	// curUnit = FindNextPlayerUnit(); // 아직 턴 안 쓴 유닛
	
	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	if (pc)
	{
		pc->Possess(curUnit);
		curUnit->OnTurnStart(); // FSM Enable
	}
}

void ATurnManager::StartEnemyTurn()
{
	// 이쪽에는 예외 처리 하면 좋을 듯
	if (auto* pawn = Cast<ABaseEnemy>(curUnit))
	{
		UE_LOG(LogTemp, Warning, TEXT("pawn is Enemy %s "),
		       *pawn->GetActorNameOrLabel());
		// FSM 활성화
		if (curUnit)
		{
			curUnit->OnTurnStart();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("pawn is Not Enemy"));
	}
}

void ATurnManager::StartNextEnemyTurn()
{
	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	if (pc)
	{
		pc->Possess(curUnit);
		curUnit->OnTurnStart(); // FSM Enable
	}
}
