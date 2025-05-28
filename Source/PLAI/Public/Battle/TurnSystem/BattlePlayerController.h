// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BattlePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API ABattlePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	
public:
	FTimerHandle timerHandle;
	
	void NotifiyReady();
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_NotifyReady();
	UFUNCTION(Client, Reliable)
	void Client_StopReadyTimer();
	void SetViewTargetMyPawn(APawn* myPawn);
};
