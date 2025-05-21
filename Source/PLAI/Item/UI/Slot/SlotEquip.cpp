// Fill out your copyright notice in the Description page of Project Settings.


#include "SlotEquip.h"

#include "Blueprint/DragDropOperation.h"
#include "Components/VerticalBox.h"
#include "PLAI/Item/Item/ItemMaster.h"
#include "PLAI/Item/Item/ItemObject.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Inventory/EquipInven/EquipInven.h"

void USlotEquip::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
	APlayerController* Pc = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	if (Pc->GetPawn()->IsLocallyControlled())
	{  UE_LOG(LogTemp,Warning,TEXT("SlotEquip::NativeOnDrop: 플레이어 캐스팅 성공 이름은? %s"),*Pc->Player->GetName());
		ATestPlayer* Player = Cast<ATestPlayer>(Pc->GetPawn());
		Player->InvenComp->Server_EquipItem(ItemObject->ItemStructTable,SlotType); }
}

FReply USlotEquip::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		// UE_LOG(LogTemp, Display, TEXT("Slot 테이블 Item id %s"),*ItemStructTable.Item_Id)
		// UE_LOG(LogTemp, Display, TEXT("Slot 테이블 Item_Attack%d"),ItemStructTable.ItemStructStat.item_ATK)
		// UE_LOG(LogTemp, Display, TEXT("Slot 테이블 Item_Defense%d"),ItemStructTable.ItemStructStat.item_DEF)
		APlayerController* Pc = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
		if (Pc->GetPawn()->IsLocallyControlled())
		{
			ATestPlayer* Player = Cast<ATestPlayer>(Pc->GetPawn());
			Player->InvenComp->Server_UnEquip(SlotType);
		}
	}
	return Super::NativeOnMouseButtonDown(MyGeometry, MouseEvent);
}

bool USlotEquip::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,UDragDropOperation* InOperation)
{
	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
	if (ItemObject->ItemStructTable.ItemTop != 1)
	{ UE_LOG(LogTemp,Warning,TEXT("SlotEquip::NativeOnDrop: Item index is not 1 아이템 인덱스머여 %d"),
	  ItemObject->ItemStructTable.ItemTop); return false; }
	
	if (static_cast<int32>(SlotType) != ItemObject->ItemStructTable.ItemIndex)
    {
		UE_LOG(LogTemp,Warning,TEXT("SlotEquip 슬롯인덱스 %d Item인덱스 %d"),SlotType,ItemObject->ItemStructTable.ItemIndex)
    	UE_LOG(LogTemp,Warning,TEXT("SlotEquip 나랑 안맞아 응 %s"),*UEnum::GetValueAsString(SlotType)) return false;
    }
	
	APlayerController* Pc = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	if (Pc->GetPawn()->IsLocallyControlled())
	{
		UE_LOG(LogTemp,Warning,TEXT("SlotEquip::NativeOnDrop: 플레이어 캐스팅 성공 이름은? %s"),*Pc->Player->GetName());
		ATestPlayer* Player = Cast<ATestPlayer>(Pc->GetPawn());
		
		Player->InvenComp->Server_UnEquip(SlotType);

		Player->InvenComp->Server_EquipItem(ItemObject->ItemStructTable,SlotType); }
	
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

// if (static_cast<int32>(SlotType) == EquipInven->LeftBox->GetChildIndex(this)
// 	&& ItemStructTable.ItemIndex != static_cast<int32>(SlotType))
	
// if (SlotType == EquipSlotType::Weapon && ItemObject->ItemStructTable.ItemIndex != 0)
// { UE_LOG(LogTemp,Warning,TEXT("SlotEquip: 웨폰 안맞네")) return false; }
//
// if (SlotType == EquipSlotType::Armor && ItemObject->ItemStructTable.ItemIndex != 1)
// { UE_LOG(LogTemp,Warning,TEXT("SlotEquip: 아머 안맞네")) return false; }
//
// if (SlotType == EquipSlotType::Helmet && ItemObject->ItemStructTable.ItemIndex != 2)
// { UE_LOG(LogTemp,Warning,TEXT("SlotEquip: 헬멧 안맞네")) return false; }
//
// if (SlotType == EquipSlotType::Gloves && ItemObject->ItemStructTable.ItemIndex != 3)
// { UE_LOG(LogTemp,Warning,TEXT("SlotEquip: 글러브 안맞네")) return false; }
//
// if (SlotType == EquipSlotType::Boots && ItemObject->ItemStructTable.ItemIndex != 4)
// { UE_LOG(LogTemp,Warning,TEXT("SlotEquip: 신발 안맞네")) return false; }
