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

    USlotEmpty* SlotEmpty = CreateWidget<USlotEmpty>(GetWorld(),SlotEmptyFactory);
	if (SlotEmpty)
	{
		SlotEmpty->SlotImage->SetBrush(SlotImage->GetBrush());
	}
	FSlateBrush Brush;
	Brush.SetResourceObject(nullptr);
	Brush.DrawAs = ESlateBrushDrawType::Type::NoDrawType;
	SlotImage->SetBrush(Brush);
	
    ItemObject->ItemStruct = ItemStruct;
	ItemStruct = FItemStruct();
	SlotCountUpdate(ItemStruct.ItemNum);
	
	ItemObject->SlotUi = this;
	
	DragOp->DefaultDragVisual = SlotEmpty;
	DragOp->Payload = ItemObject;
	DragOp->Pivot = EDragPivot::MouseDown;

	UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnMouseDrag"));
	OutOperation = DragOp;
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

bool USlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
	Swap(ItemStruct, ItemObject->ItemStruct);

	SlotImageUpdate();
	SlotCountUpdate(ItemStruct.ItemNum);
	
	UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnDrop"));
	return false;
	// return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void USlot::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	
	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
	ItemStruct = ItemObject->ItemStruct;
	SlotImageUpdate();
	UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnDrop 다른창에 했음"));
}
