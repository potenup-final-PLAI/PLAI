// Fill out your copyright notice in the Description page of Project Settings.


#include "SlotCre.h"

#include "Blueprint/DragDropOperation.h"
#include "PLAI/Item/Item/ItemObject.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"

FReply USlotCre::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// TArray<FName>RawNames = ItemTable->GetRowNames();
	// for (FName RawName : RawNames)
	// {
	// 	FItemStructTable* Table = ItemTable->FindRow<FItemStructTable>(RawName,TEXT("SlotCre"));
	// 	if (ItemStructTable->ItemIndex)
	// }
	
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

bool USlotCre::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                            UDragDropOperation* InOperation)
{
	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
    if (ItemObject && ItemObject->ItemStructTable.ItemTop == 3)
    {
	    UE_LOG(LogTemp,Warning,TEXT("USlotCre::크리처 들어왔음 NativeOnDrop"));
    	// ATestPlayer* TestPlayer = Cast<APlayerController>(GetWorld()->GetFirstPlayerController()->GetPawn);
    	// TestPlayer->InvenComp->Creature = ItemStructTable.Creature;
    }
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("USlotCre::크리처 안들어왔음 NativeOnDrop"));
		return false;
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}