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
	UCycleAndTurn* WBP_CycleAndTurn;
};
