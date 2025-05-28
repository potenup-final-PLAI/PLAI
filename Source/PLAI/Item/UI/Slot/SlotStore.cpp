// Fill out your copyright notice in the Description page of Project Settings.


#include "SlotStore.h"

#include "Blueprint/DragDropOperation.h"
#include "Components/Image.h"
#include "PLAI/Item/Item/ItemObject.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"

void USlotStore::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	UDragDropOperation* DragOp = NewObject<UDragDropOperation>();
	UItemObject* ItemObject = NewObject<UItemObject>();
 
	USlotEmpty* SlotEmpty = CreateWidget<USlotEmpty>(GetWorld(),SlotEmptyFactory);
	if (SlotEmpty)
	{
		SlotEmpty->SlotImage->SetBrush(SlotImage->GetBrush());
	}
	// FSlateBrush Brush;
	// Brush.SetResourceObject(nullptr);
	// Brush.DrawAs = ESlateBrushDrawType::Type::NoDrawType;
	// SlotImage->SetBrush(Brush);
	
	ItemObject->ItemStructTable = ItemStructTable;
	// ItemStructTable = FItemStructTable();
	// SlotCountUpdate(ItemStructTable.ItemNum);

	ItemObject->SlotUi = this;
	ItemObject->SlotUi->SlotType = ESLotType::Store;
    ItemObject->bBuy = true;
	
	DragOp->DefaultDragVisual = SlotEmpty;
	DragOp->Payload = ItemObject;
	DragOp->Pivot = EDragPivot::MouseDown;
 
	UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnMouseDrag"));
	OutOperation = DragOp;
	
	// Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

bool USlotStore::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                              UDragDropOperation* InOperation)
{
	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
	if (ItemObject->ItemStructTable.ItemIndex != -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("SlotStore::NativeOnDrop: ItemObject is 아이템이 있네 %d"),ItemObject->ItemStructTable.ItemGold);

		if (ItemObject->SlotUi->SlotType != ESLotType::Store)
		{
			ItemObject->SlotUi->ItemStructTable = FItemStructTable();
			ItemObject->SlotUi->SlotImageUpdate();
			if (APlayerController* Pc = GetWorld()->GetFirstPlayerController())
			{
				if (ATestPlayer* Player = Cast<ATestPlayer>(Pc->GetCharacter()))
				{ Player->InvenComp->SetGold(ItemObject->ItemStructTable.ItemGold); }
			}
		}
		
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SlotStore::NativeOnDrop: ItemObject is 아이템이 없네 %d"),ItemObject->ItemStructTable.ItemGold);
		return false;
	}
}
