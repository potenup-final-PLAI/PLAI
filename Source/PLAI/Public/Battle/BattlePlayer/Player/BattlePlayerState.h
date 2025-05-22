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

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	//--------------------Status--------------------
	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadWrite, Category="Battle")
	FBaseStatus playerStatus;
	
	//--------------------LifeState--------------------
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Battle")
	ELifeState playerLifeState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle")
	APawn* battlePawn;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle")
	ABattlePlayerState* ownPS;
};
