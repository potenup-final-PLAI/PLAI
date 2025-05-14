// Fill out your copyright notice in the Description page of Project Settings.


#include "SlotStore.h"

#include "Blueprint/DragDropOperation.h"
#include "PLAI/Item/Item/ItemObject.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"

bool USlotStore::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                              UDragDropOperation* InOperation)
{
	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
	if (ItemObject->ItemStructTable.ItemIndex != -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("SlotStore::NativeOnDrop: ItemObject is 아이템이 있네 %d"),ItemObject->ItemStructTable.ItemGold);
	
		ItemObject->SlotUi->ItemStructTable = FItemStructTable();
		ItemObject->SlotUi->SlotImageUpdate();
		
		if (APlayerController* Pc = GetWorld()->GetFirstPlayerController())
		{
			if (ATestPlayer* Player = Cast<ATestPlayer>(Pc->GetCharacter()))
			{
				Player->InvenComp->SetGold(ItemObject->ItemStructTable.ItemGold);
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
