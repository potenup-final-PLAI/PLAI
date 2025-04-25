// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slot.h"
#include "SlotQuick.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API USlotQuick : public USlot
{
	GENERATED_BODY()
	
public:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;;
};
