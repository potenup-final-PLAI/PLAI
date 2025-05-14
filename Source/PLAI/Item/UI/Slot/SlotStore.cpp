// Fill out your copyright notice in the Description page of Project Settings.


#include "SlotStore.h"

bool USlotStore::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
