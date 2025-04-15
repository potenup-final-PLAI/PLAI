// Fill out your copyright notice in the Description page of Project Settings.


#include "Slot.h"

#include "Blueprint/DragDropOperation.h"
#include "PLAI/Item/Item/ItemObject.h"


FReply USlot::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnMouseButtonDown"));
	return Super::NativeOnMouseButtonDown(MyGeometry, MouseEvent);
}

void USlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                 UDragDropOperation*& OutOperation)
{
	UDragDropOperation* DragOp = NewObject<UDragDropOperation>();
	UItemObject* ItemObject = NewObject<UItemObject>();

	UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnMouseDrag"));
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

bool USlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnDrop"));
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
