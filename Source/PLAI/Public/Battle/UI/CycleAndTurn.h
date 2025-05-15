// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CycleAndTurn.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UCycleAndTurn : public UUserWidget
{
	GENERATED_BODY()

public:
	// Cycle과 Player 또는 Enemy중 누가 Turn을 잡았는지 UI에 띄우기 위해
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* txt_Cycle;
	
	void SetCycleText(int32 cycle);
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* txt_turn;
	
	void SetTurnText(FString unit);
	
};
