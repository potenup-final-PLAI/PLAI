// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slot.h"
#include "SlotCre.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API USlotCre : public USlot
{
	GENERATED_BODY()
public:
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

public:
	void SpawnCreature(FItemStructTable ItemStructTab);
};
