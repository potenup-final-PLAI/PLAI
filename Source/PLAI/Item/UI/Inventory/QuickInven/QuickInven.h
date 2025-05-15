// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PLAI/Item/UI/Slot/SlotQuick.h"
#include "QuickInven.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UQuickInven : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* QuickSlotBox;

    UPROPERTY(EditAnywhere)
	TSubclassOf<USlotQuick> QuickSlotFactory;
	
	UPROPERTY(EditAnywhere)
	class USlotQuick* QuickSlot;

	virtual void NativeConstruct() override;
};
