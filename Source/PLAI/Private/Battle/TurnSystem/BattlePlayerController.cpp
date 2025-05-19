// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/TurnSystem/BattlePlayerController.h"

#include "Enemy/BaseEnemy.h"


void ABattlePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
}

void ABattlePlayerController::ClientRPC_SetViewTargetMyPawn_Implementation(APawn* myPawn)
{
	// 빠르게 갔다가 천천히 도착 하는 느낌
	SetViewTargetWithBlend(myPawn, 1.0f, VTBlend_EaseInOut, 4.0f, true);
}
