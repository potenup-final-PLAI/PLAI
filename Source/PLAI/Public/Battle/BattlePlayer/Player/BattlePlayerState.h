// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
#include "Battle/Util/BattleType/BattleTypes.h"
#include "BattlePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API ABattlePlayerState : public ABasePlayerState
{
	GENERATED_BODY()

public:
	//--------------------Status--------------------
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Battle")
	FBaseStatus playerStatus;
	
	//--------------------LifeState--------------------
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Battle")
	ELifeState playerLifeState;
};
