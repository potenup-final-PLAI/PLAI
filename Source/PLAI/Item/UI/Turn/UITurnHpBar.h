// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PLAI/Item/TestPlayer/TurnPlayer.h"
#include "UITurnHpBar.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUITurnHpBar : public UUserWidget
{
	GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
	class UTextBlock* MaxHp;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentHp;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HpBar;
	
	void SetHpBar(FTurnPlayerStruct TurnPlayerStruct);
};
