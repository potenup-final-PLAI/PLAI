// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PLAI/Item/UI/Slot/SlotEquip.h"
#include "EquipInven.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UEquipInven : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<USlotEquip> SlotEquipFactory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USlotEquip* SlotEquip;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UVerticalBox* LeftBox;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UVerticalBox* RightBox;

	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
