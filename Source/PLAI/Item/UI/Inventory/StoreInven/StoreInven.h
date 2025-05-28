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
	class UWrapBox* EquipBox;

	UPROPERTY(meta = (BindWidget))
	class UWrapBox* ConsumeBox;

	UPROPERTY(meta = (BindWidget))
	class UWrapBox* LegendaryBox;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* Equip;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* Consume;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* Legendary;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Equip;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Consume;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Legendary;

	UPROPERTY(EditAnywhere)
	UDataTable* ItemTable;
	
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnButtonEquip();

	UFUNCTION()
	void OnButtonConsume();

	UFUNCTION()
	void OnButtonLegendary();
};
