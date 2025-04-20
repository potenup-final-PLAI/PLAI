// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PLAI/Item/UI/Slot/SlotStore.h"
#include "StoreInven.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UStoreInven : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<USlotStore> SlotStoreFactory;

	UPROPERTY(meta = (BindWidget))
	class UWrapBox* WrapBox;

	virtual void NativeConstruct() override;
};
