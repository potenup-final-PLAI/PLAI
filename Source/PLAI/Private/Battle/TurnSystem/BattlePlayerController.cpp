// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/TurnSystem/BattlePlayerController.h"

#include "Battle/TurnSystem/PhaseManager.h"
#include "Player/BattlePlayer.h"


void ABattlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &ABattlePlayerController::NotifiyReady, 0.2f, true);
	}
}

void ABattlePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ABattlePlayerController::NotifiyReady()
{
	ServerRPC_NotifyReady();
}

void ABattlePlayerController::ServerRPC_NotifyReady_Implementation()
{
	if (AUPhaseManager* phaseManager = Cast<AUPhaseManager>(GetWorld()->GetGameState()))
	{
		phaseManager->PlayerReady(this);
		// UE_LOG(LogTemp, Warning, TEXT("Player ready %s"), *this->GetActorNameOrLabel());
	}
}

void ABattlePlayerController::Client_StopReadyTimer_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(timerHandle);
}

void ABattlePlayerController::SetViewTargetMyPawn(APawn* myPawn)
{
	if (IsLocalController())
	{
		// 빠르게 갔다가 천천히 도착 하는 느낌
		SetViewTargetWithBlend(myPawn, 1.0f, VTBlend_EaseInOut, 4.0f, true);
	}
}
