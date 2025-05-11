// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BattleHUD.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API ABattleHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

	UPROPERTY()
	class UMainBattleUI* mainUI;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMainBattleUI> mainUIFactory;
	
	
	
};
