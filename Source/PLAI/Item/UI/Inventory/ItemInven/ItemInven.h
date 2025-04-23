// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PLAI/Item/UI/Slot/Slot.h"
#include "ItemInven.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UItemInven : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UWrapBox* WrapBox;

	UPROPERTY(EditAnywhere)
	TSubclassOf<USlot>SlotFactory;

	UPROPERTY(EditAnywhere)
	class USlot* SlotUi;
    // 추후에 바뀔 가능성 Ui 위치
	UPROPERTY(meta = (BindWidget))
	class UItemGold* WbpItemGold;

public:
	virtual void NativeConstruct() override;
};
