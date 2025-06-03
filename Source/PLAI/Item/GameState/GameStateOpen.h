// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "PLAI/Item/TestPlayer/TurnPlayer.h"
#include "PLAI/Item/Turn/TurnMoster/TurnMonster.h"
#include "GameStateOpen.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API AGameStateOpen : public AGameStateBase
{
	GENERATED_BODY()

public:
	AGameStateOpen();

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	TArray<ATurnPlayer*> TurnPlayers;

	UPROPERTY(EditAnywhere)
	TArray<ATurnMonster*> TurnMonsters;

	void FindPlayerTurn();
	void NextPlayerTurn(ATurnPlayer* TurnPlayer);
	
	void FindMonsterTurn();
	void NextMonsterTurn();
	
	UPROPERTY(EditAnywhere)
	TArray<AActor*>MiniMapGuideActors;

	void FindMiniMapGuideIcon();

	UPROPERTY(EditAnywhere)
	FTimerHandle TimerHandle;
	UPROPERTY(EditAnywhere)
	float TimerSecond;
};
