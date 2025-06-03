// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WhoTurn.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UWhoTurn : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* WhoTurnAnimation;

	void PlayWhoTurnAnimation();
	
};
