// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorldDamageUI.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UWorldDamageUI : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_Damage;

	void SetDamageText(const int32 damage);
};
