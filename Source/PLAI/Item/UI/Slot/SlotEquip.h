// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slot.h"
#include "SlotEquip.generated.h"

UENUM(BlueprintType)
enum class EquipSlotType : uint8
{
	Weapon UMETA(DisplayName = "Weapon"),
	Armor UMETA(DisplayName = "Armor"),
	Helmet UMETA(DisplayName = "Helmet"),
	Gloves UMETA(DisplayName = "Gloves"),
	Boots UMETA(DisplayName = "Boots"),
};

/**
 * 
 */
UCLASS()
class PLAI_API USlotEquip : public USlot
{
	GENERATED_BODY()

public:
	// 장비창 내장비아이템 담아놓기
    EquipSlotType SlotType;
	
    UPROPERTY(EditAnywhere)
	class AItemMaster* ItemMaster;

	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
