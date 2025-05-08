// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UiTurnRewardImage.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUiTurnRewardImage : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget = "Widget"))
	class UImage* RewardImage;
};
