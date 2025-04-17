// Fill out your copyright notice in the Description page of Project Settings.


#include "SlotEquip.h"

#include "Blueprint/DragDropOperation.h"
#include "PLAI/Item/Item/ItemObject.h"

bool USlotEquip::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                              UDragDropOperation* InOperation)
{
	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
	if (ItemObject->ItemStruct.ItemTop != 1)
	{
		UE_LOG(LogTemp,Warning,TEXT("SlotEquip::NativeOnDrop: Item index is not 1 아이템 인덱스머여 %d"),
			ItemObject->ItemStruct.ItemTop);
		
		return false;
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
