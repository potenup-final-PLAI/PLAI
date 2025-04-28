// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/BattlePlayer/BaseBattlePawn.h"
#include "BattlePlayer.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API ABattlePlayer : public ABaseBattlePawn
{
	GENERATED_BODY()

protected:
	virtual void PossessedBy(AController* NewController) override;	
public:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditAnywhere)
	class UInputMappingContext* IMC_Player;
	UPROPERTY(EditAnywhere)
	class UInputAction* IA_Move;
};
