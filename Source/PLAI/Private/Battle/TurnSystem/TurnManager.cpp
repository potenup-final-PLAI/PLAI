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
	if (curTurnState == ETurnState::TurnEnd && curTurnState == ETurnState::PlayerTurn && curTurnState == ETurnState::EnemyTurn) return;
	UE_LOG(LogTemp, Warning, TEXT("TurnManager : Start Player Turn"));
	SetTurnState(ETurnState::PlayerTurn);
	if (ABattlePlayer* playerPawn = Cast<ABattlePlayer>(curUnit))
	{
		UE_LOG(LogTemp, Warning, TEXT("playerPawn is Player %s"), *playerPawn->GetActorNameOrLabel());
		
		if (APlayerController* pc = GetWorld()->GetFirstPlayerController())
		{
			// 빠르게 갔다가 천천히 도착 하는 느낌
			pc->SetViewTargetWithBlend(curUnit, 1.0f, VTBlend_EaseInOut, 4.0f, true);
			// 좀 더 부드럽게 왔다갔다 하는 느낌
			// pc->SetViewTargetWithBlend(curUnit, 1.0f, VTBlend_Cubic, 4.0f, true);

			FTimerHandle possessHandle;
			GetWorld()->GetTimerManager().ClearTimer(possessHandle);
			// 이후에 값이 변경 및 삭제 될 수 있기 때문에 값 복사로 가져와서 람다 내에서 사용
			GetWorld()->GetTimerManager().SetTimer(possessHandle, FTimerDelegate::CreateLambda([=]()
			{
				pc->Possess(playerPawn);
				playerPawn->OnTurnStart();
				UE_LOG(LogTemp, Warning, TEXT("possess unit %s"), *playerPawn->GetActorNameOrLabel());
			}), 1.0f, false);
		}
	}
}


void ATurnManager::StartEnemyTurn()
{
	if (curTurnState == ETurnState::TurnEnd && curTurnState == ETurnState::PlayerTurn && curTurnState == ETurnState::EnemyTurn) return;
	UE_LOG(LogTemp, Warning, TEXT("TurnManager : Start Enemy Turn"));
	SetTurnState(ETurnState::EnemyTurn);
	
	
	if (ABaseEnemy* pawn = Cast<ABaseEnemy>(curUnit))
	{
		UE_LOG(LogTemp, Warning, TEXT("pawn is Enemy %s "),
		       *pawn->GetActorNameOrLabel());
		// FSM 활성화
		if (curUnit)
		{
			if (APlayerController* pc = GetWorld()->GetFirstPlayerController())
			{
				// 빠르게 갔다가 천천히 도착 하는 느낌
				pc->SetViewTargetWithBlend(curUnit, 1.0f, VTBlend_EaseInOut, 4.0f, true);

				// 1. ClearTimer로 타이머 초기화 하고 시작
				// 2. Set은 BeginPlay에서 하고 GetTimer로 가져와서 진행
				
				FTimerHandle possessHandle;
				GetWorld()->GetTimerManager().ClearTimer(possessHandle);
				// 이후에 값이 변경 및 삭제 될 수 있기 때문에 값 복사로 가져와서 람다 내에서 사용
				GetWorld()->GetTimerManager().SetTimer(possessHandle, FTimerDelegate::CreateLambda([=, this]()
				{
					pc->Possess(curUnit);
					curUnit->OnTurnStart();
					UE_LOG(LogTemp, Warning, TEXT("possess unit %s"), *curUnit->GetActorNameOrLabel());
				}), 1.0f, false);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("pawn is Not Enemy"));
	}
}
