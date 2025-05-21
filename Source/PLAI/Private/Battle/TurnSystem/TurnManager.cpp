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
#include "Net/UnrealNetwork.h"
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

	DOREPLIFETIME(ATurnManager, curTurnState);
	DOREPLIFETIME(ATurnManager, curUnit);
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
		UE_LOG(LogTemp, Warning, TEXT("curTurnState = PlayerTurn"));
		break;
	case ETurnState::EnemyTurn:
		UE_LOG(LogTemp, Warning, TEXT("curTurnState = PlayerTurn"));
		break;
	case ETurnState::TurnEnd:
		break;
	default:
		break;
	}
}

void ATurnManager::MutliCastRPC_UpdateWhoTurn_Implementation(const FString& turnUnitName)
{
	// 턴 유닛 이름 UI에 세팅
	if (phaseManager->pc && phaseManager->hud && phaseManager->hud->mainUI && phaseManager->hud->mainUI->WBP_CycleAndTurn)
	{
		phaseManager->hud->mainUI->WBP_CycleAndTurn->SetTurnText(turnUnitName);
	}
}


void ATurnManager::StartPlayerTurn()
{
	if (!HasAuthority() || curTurnState != ETurnState::None)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("TurnManager : Start Player Turn"));
	// Player Turn으로 state Update
	SetTurnState(ETurnState::PlayerTurn);
	// Turn UI 업데이트
	FString s = Cast<ABattlePlayer>(curUnit) ? TEXT("Player") : TEXT("Enemy");
	
	MutliCastRPC_UpdateWhoTurn(s);
	
	if (ABattlePlayer* playerPawn = Cast<ABattlePlayer>(curUnit))
	{
		if (ABattlePlayerController* battlePC = Cast<ABattlePlayerController>(playerPawn->GetOwner()))
		{
			FTimerHandle possessHandle;
			GetWorld()->GetTimerManager().ClearTimer(possessHandle);

			GetWorld()->GetTimerManager().SetTimer(possessHandle, FTimerDelegate::CreateLambda([=, this]()
			{
				battlePC->Possess(playerPawn);
				if (curUnit->IsValidLowLevelFast())
				{
					UE_LOG(LogTemp, Warning,TEXT("Safe to call OnTurnStart on %s"),*curUnit->GetName());
					auto* playerCamera = battlePC->GetPawnOrSpectator();
					curUnit->OnTurnStart();
					MulticastRPC_CameraChange(playerCamera);
				}
			}), 1.0f, false);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("can't find playerPawn"));
	}
}


void ATurnManager::StartEnemyTurn()
{
	if (curTurnState != ETurnState::None)
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("TurnManager : Start Enemy Turn"));
	// EnemyTurn으로 State 업데이트
	SetTurnState(ETurnState::EnemyTurn);
	FString s = Cast<ABaseEnemy>(curUnit) ? TEXT("Player") : TEXT("Enemy");
	// Turn UI 업데이트 
	MutliCastRPC_UpdateWhoTurn(s);
	
	if (ABaseEnemy* pawn = Cast<ABaseEnemy>(curUnit))
	{
		UE_LOG(LogTemp, Warning, TEXT("pawn is Enemy %s "), *pawn->GetActorNameOrLabel());
		// FSM 활성화
		if (curUnit)
		{
			if (APlayerController* pc = GetWorld()->GetFirstPlayerController())
			{
				FTimerHandle possessHandle;
				GetWorld()->GetTimerManager().ClearTimer(possessHandle);
				// 이후에 값이 변경 및 삭제 될 수 있기 때문에 값 복사로 가져와서 람다 내에서 사용
				GetWorld()->GetTimerManager().SetTimer(
					possessHandle, FTimerDelegate::CreateLambda([=, this]()
					{
						if (pc && curUnit)
						{
							pc->Possess(curUnit);
							auto* playerCamera = pc->GetPawnOrSpectator();
							curUnit->OnTurnStart();
							MulticastRPC_CameraChange(curUnit);
							UE_LOG(LogTemp, Warning, TEXT("possess unit %s"),*curUnit->GetActorNameOrLabel());
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

void ATurnManager::MulticastRPC_CameraChange_Implementation(class APawn* target)
{
	if (auto* pc = Cast<ABattlePlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		pc->SetViewTargetMyPawn(target);
	}
}
