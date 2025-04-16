// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlotEmpty.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API USlotEmpty : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* SlotImage;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* SlotCount;
};
