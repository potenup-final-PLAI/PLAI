// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CycleAndTurn.h"
#include "Blueprint/UserWidget.h"
#include "MainBattleUI.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UMainBattleUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
public:
	UPROPERTY(meta = (BindWidget))
	class UCycleAndTurn* WBP_CycleAndTurn;

	UPROPERTY(meta = (BindWidget))
	class UBattlePlayerInfoUI* WBP_Player;
};
