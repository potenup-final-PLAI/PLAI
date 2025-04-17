// Fill out your copyright notice in the Description page of Project Settings.


#include "SlotEquip.h"

#include "Blueprint/DragDropOperation.h"
#include "PLAI/Item/Item/ItemMaster.h"
#include "PLAI/Item/Item/ItemObject.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"

FReply USlotEquip::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		APlayerController* Pc = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
		if (Pc->IsLocalController())
		{
			UE_LOG(LogTemp,Warning,TEXT("SlotEquip::NativeOnMouse 왼쪽버튼다운: 플레이어 캐스팅 성공 이름은? %s"),*GetName());
			ATestPlayer* Player = Cast<ATestPlayer>(Pc->GetPawn());
			if (Player->InvenComp->ItemWeapon)
			{ Player->InvenComp->ItemWeapon->Destroy();
				Player->InvenComp->ItemWeapon = nullptr; }
		}
	}
	return Super::NativeOnMouseButtonDown(MyGeometry, MouseEvent);
}

bool USlotEquip::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,UDragDropOperation* InOperation)
{
	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
	if (ItemObject->ItemStruct.ItemTop != 1)
	{ UE_LOG(LogTemp,Warning,TEXT("SlotEquip::NativeOnDrop: Item index is not 1 아이템 인덱스머여 %d"),
	  ItemObject->ItemStruct.ItemTop); return false; }

	if (SlotType == EquipSlotType::Weapon && ItemObject->ItemStruct.ItemIndex != 0)
	{ UE_LOG(LogTemp,Warning,TEXT("SlotEquip: 웨폰 안맞네")) return false; }
	
	if (SlotType == EquipSlotType::Armor && ItemObject->ItemStruct.ItemIndex != 1)
	{ UE_LOG(LogTemp,Warning,TEXT("SlotEquip: 아머 안맞네")) return false; }

	if (SlotType == EquipSlotType::Helmet && ItemObject->ItemStruct.ItemIndex != 2)
	{ UE_LOG(LogTemp,Warning,TEXT("SlotEquip: 헬멧 안맞네")) return false; }

	if (SlotType == EquipSlotType::Gloves && ItemObject->ItemStruct.ItemIndex != 3)
	{ UE_LOG(LogTemp,Warning,TEXT("SlotEquip: 글러브 안맞네")) return false; }

	if (SlotType == EquipSlotType::Boots && ItemObject->ItemStruct.ItemIndex != 4)
	{ UE_LOG(LogTemp,Warning,TEXT("SlotEquip: 신발 안맞네")) return false; }
	
	APlayerController* Pc = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	if (Pc->IsLocalController())
	{  UE_LOG(LogTemp,Warning,TEXT("SlotEquip::NativeOnDrop: 플레이어 캐스팅 성공 이름은? %s"),*Pc->Player->GetName());
		ATestPlayer* Player = Cast<ATestPlayer>(Pc->GetPawn());
		Player->InvenComp->EquipItem(ItemObject->ItemStruct,this); }
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
