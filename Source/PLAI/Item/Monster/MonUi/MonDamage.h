// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonDamage.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UMonDamage : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget = "Widget"))
	class UTextBlock* Damage;

    UPROPERTY(meta =(BindWidgetAnim), Transient)
	UWidgetAnimation* DamageUiAnimation;
	
	virtual void NativeConstruct() override;
};
