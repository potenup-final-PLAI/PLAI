// Fill out your copyright notice in the Description page of Project Settings.


#include "Slot.h"

#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PLAI/Item/Item/ItemObject.h"


void USlot::SlotCountUpdate(const int32 Count)
{
	SlotCount->SetText(FText::AsNumber(Count));
}

void USlot::SlotImageUpdate()
{
	FSlateBrush Brush;
	Brush.SetResourceObject(ParentItem->ItemStructTop.ItemMeshTops[ItemStruct.ItemTop].ItemMeshIndexes[ItemStruct.ItemIndex].
		ItemMeshTypes[ItemStruct.ItemIndexType].Textures[ItemStruct.ItemIndexDetail]);
	SlotImage->SetBrush(Brush);
}

void USlot::NativeConstruct()
{
	Super::NativeConstruct();
	ParentItem = ItemFactory->GetDefaultObject<AItem>();
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
		return UWidgetBlueprintLibrary::DetectDragIfPressed(MouseEvent, this,
			EKeys::LeftMouseButton).NativeReply;
	}
	return Super::NativeOnMouseButtonDown(MyGeometry, MouseEvent);
}

void USlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                 UDragDropOperation*& OutOperation)
{
	UDragDropOperation* DragOp = NewObject<UDragDropOperation>();
	UItemObject* ItemObject = NewObject<UItemObject>();
	
	ItemObject->Texture = Texture;

	DragOp->DefaultDragVisual = this;
	DragOp->Payload = ItemObject;
	DragOp->Pivot = EDragPivot::MouseDown;

	UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnMouseDrag"));
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	
	OutOperation = DragOp;
}

bool USlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	UItemObject* ItemObject = Cast<UItemObject>(InOperation);
	Texture = ItemObject->Texture;

	FSlateBrush Brush;
	Brush.SetResourceObject(Texture);
	SlotImage->SetBrush(Brush);
	
	UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnDrop"));
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
