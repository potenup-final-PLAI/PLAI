// Fill out your copyright notice in the Description page of Project Settings.


#include "SlotEquip.h"

#include "Blueprint/DragDropOperation.h"
#include "PLAI/Item/Item/ItemObject.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"

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
	APlayerController* Pc = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	if (Pc->IsLocalController())
	{
		UE_LOG(LogTemp,Warning,TEXT("SlotEquip::NativeOnDrop: 플레이어 캐스팅 성공 이름은? %s"),*Pc->Player->GetName());
		ATestPlayer* Player = Cast<ATestPlayer>(Pc->GetPawn());
		Player->InvenComp->EquipItem(ItemObject->ItemStruct);
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
