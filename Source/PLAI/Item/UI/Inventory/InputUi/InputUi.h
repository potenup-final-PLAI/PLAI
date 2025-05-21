// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InputUi.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UInputUi : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RotateViewText;
	
	void InputUiHidden();

	float CurrentOpacity = 1.f;
	
	float SignDegree = 0.f;
	
	FTimerHandle TimerHandle;
};
