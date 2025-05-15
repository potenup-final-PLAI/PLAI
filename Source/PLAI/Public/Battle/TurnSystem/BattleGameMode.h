// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API ABattleGameMode : public AGameModeBase
{
	GENERATED_BODY()
protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
