// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UiTurnRewardImage.h"
#include "Blueprint/UserWidget.h"
#include "UiTurnReward.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUiTurnReward : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUiTurnRewardImage>UiTurnRewardImageFactory;
	
    UPROPERTY(EditAnywhere)
	class UUiTurnRewardImage* UiTurnRewardImage;
	
	UPROPERTY(meta = (BindWidget))
	class UWrapBox* RewardBox;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RewardGold;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RewardExp;
};
