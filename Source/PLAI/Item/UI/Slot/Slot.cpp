// Fill out your copyright notice in the Description page of Project Settings.


#include "Slot.h"

#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/TextBlock.h"
#include "PLAI/Item/Item/ItemObject.h"


void USlot::SlotCountUpdate(const int32 Count)
{
	SlotCount->SetText(FText::AsNumber(Count));
}

FReply USlot::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (ItemStruct.ItemTop == -1)
	{
		UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnMouseButtonDown 슬롯 템 없음"));
		return Super::NativeOnMouseButtonDown(MyGeometry, MouseEvent);
	}

	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		UE_LOG(LogTemp, Display, TEXT("Slot::왼쪽마우스 NativeOnMouseButtonDown"));
		return UWidgetBlueprintLibrary::DetectDragIfPressed(MouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	return Super::NativeOnMouseButtonDown(MyGeometry, MouseEvent);
}

void USlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                 UDragDropOperation*& OutOperation)
{
	UDragDropOperation* DragOp = NewObject<UDragDropOperation>();
	UItemObject* ItemObject = NewObject<UItemObject>();

	DragOp->DefaultDragVisual = this;

	UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnMouseDrag"));
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

bool USlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnDrop"));
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
