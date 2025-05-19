// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/TurnSystem/TurnManager.h"

#include "BaseBattlePawn.h"
#include "Battle/TurnSystem/BattlePlayerController.h"
#include "Battle/TurnSystem/PhaseManager.h"
#include "Battle/UI/BattleHUD.h"
#include "Battle/UI/MainBattleUI.h"
#include "Enemy/BaseEnemy.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Player/BattlePlayer.h"

DEFINE_LOG_CATEGORY(TPS);

// Sets default values
ATurnManager::ATurnManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ATurnManager::BeginPlay()
{
	Super::BeginPlay();
	phaseManager = Cast<AUPhaseManager>(GetWorld()->GetGameState());
}

void ATurnManager::GetLifetimeReplicatedProps(
	TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
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

void ATurnManager::UpdateWhoTurn()
{
	// 턴 유닛 이름 UI에 세팅
	if (phaseManager->pc && phaseManager->hud && phaseManager->hud->mainUI && phaseManager->hud->mainUI->WBP_CycleAndTurn)
	{
		FString s = Cast<ABattlePlayer>(phaseManager->turnManager->curUnit)
						? TEXT("Player")
						: TEXT("Enemy");
		phaseManager->hud->mainUI->WBP_CycleAndTurn->SetTurnText(s);
	}
}

void ATurnManager::OnRep_UpdateWhoTurn()
{
	UpdateWhoTurn();
}

void ATurnManager::MulticastRPC_StartPlayerTurn_Implementation()
{
	if (curTurnState != ETurnState::None)
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("TurnManager : Start Player Turn"));
	// Player 턴 시작
	SetTurnState(ETurnState::PlayerTurn);

	if (ABattlePlayer* playerPawn = Cast<ABattlePlayer>(curUnit))
	{
		UE_LOG(LogTemp, Warning, TEXT("playerPawn is Player %s"),
		       *playerPawn->GetActorNameOrLabel());

		if (ABattlePlayerController* pc = Cast<ABattlePlayerController>(playerPawn->GetController()))
		{
			FTimerHandle possessHandle;
			GetWorld()->GetTimerManager().ClearTimer(possessHandle);

			// 이후에 값이 변경 및 삭제 될 수 있기 때문에 값 복사로 가져와서 람다 내에서 사용
			GetWorld()->GetTimerManager().SetTimer(
				possessHandle, FTimerDelegate::CreateLambda([=, this]()
				{
					if (pc && playerPawn)
					{
						pc->Possess(playerPawn);
						if (curUnit->IsValidLowLevelFast())
						{
							UE_LOG(LogTemp, Warning, TEXT("Safe to call OnTurnStart on %s"), *curUnit->GetName());
							// 누구턴인지 UI 업데이트
							if (HasAuthority()) UpdateWhoTurn();
							
							curUnit->OnTurnStart();
							// Client RPC로 시점 전환
							if (ABattlePlayerController* myPC = Cast<ABattlePlayerController>(pc))
							{
								myPC->ClientRPC_SetViewTargetMyPawn(playerPawn);
							}
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("Not Safe to call OnTurnStart"));
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("haven't pc or playerPawn"));
					}
				}), 1.0f, false);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("can't find playerPawn"));
		}
	}
}


void ATurnManager::MulticastRPC_StartEnemyTurn_Implementation()
{
	if (curTurnState != ETurnState::None)
	{
		return;
	}
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
				pc->SetViewTargetWithBlend(curUnit, 1.0f, VTBlend_EaseInOut,
				                           4.0f, true);
				
				FTimerHandle possessHandle;
				GetWorld()->GetTimerManager().ClearTimer(possessHandle);
				// 이후에 값이 변경 및 삭제 될 수 있기 때문에 값 복사로 가져와서 람다 내에서 사용
				GetWorld()->GetTimerManager().SetTimer(
					possessHandle, FTimerDelegate::CreateLambda([=, this]()
					{
						if (pc && curUnit)
						{
							pc->Possess(curUnit);
							curUnit->OnTurnStart();
							UE_LOG(LogTemp, Warning, TEXT("possess unit %s"),
							       *curUnit->GetActorNameOrLabel());
						}
					}), 1.0f, false);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("pawn is Not Enemy"));
	}
}
