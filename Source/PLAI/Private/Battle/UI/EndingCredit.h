// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndingCredit.generated.h"


/**
 * 
 */
UCLASS()
class PLAI_API UEndingCredit : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* PaidIn;
	
	UFUNCTION(NetMulticast, Reliable)
	void Mulitcast_PaidIn();
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* EndingCredit;

	UFUNCTION()
	void PlayEndingCredit();
};
