// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BattlePlayerState.h"

#include "Net/UnrealNetwork.h"

void ABattlePlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABattlePlayerState, playerStatus);
}
