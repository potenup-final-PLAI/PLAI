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
		if (Pc->GetPawn()->IsLocallyControlled())
		{
			UE_LOG(LogTemp,Warning,TEXT("SlotEquip::NativeOnMouse 왼쪽버튼다운: 플레이어 캐스팅 성공 이름은? %s"),*GetName());
			ATestPlayer* Player = Cast<ATestPlayer>(Pc->GetPawn());
			
			if (SlotType == EquipSlotType::Weapon && Player->InvenComp->ItemWeapon)
			{  AItem* Item = Cast<AItem>(Player->InvenComp->ItemWeapon);
				if (Item)
				{
					UE_LOG(LogTemp,Warning,TEXT("SlotEquip::NativeOnMouse 아이템 캐스팅 성공 UnEquip함수호출 성공 "));
					Player->InvenComp->Server_UnEquip(Item);
				}
				else
				{
					UE_LOG(LogTemp,Warning,TEXT("SlotEquip::NativeOnMouse UnEquip함수호출 실패 "));
				}
				// Player->InvenComp->ItemWeapon->Destroy();
				// Player->InvenComp->ItemWeapon = nullptr;
			}
			
			if (SlotType == EquipSlotType::Armor && Player->InvenComp->ItemArmor)
			{ Player->InvenComp->ItemArmor ->Destroy();
				Player->InvenComp->ItemArmor = nullptr; }
			
			if (SlotType == EquipSlotType::Helmet && Player->InvenComp->ItemHelmet)
			{ Player->InvenComp->ItemHelmet ->Destroy();
				Player->InvenComp->ItemHelmet = nullptr; }
			
			if (SlotType == EquipSlotType::Gloves && Player->InvenComp->ItemGlove)
			{ Player->InvenComp->ItemGlove ->Destroy();
				Player->InvenComp->ItemGlove = nullptr; }
			
			if (SlotType == EquipSlotType::Boots && Player->InvenComp->Itemboots)
			{
				AItem* Item = Cast<AItem>(Player->InvenComp->Itemboots);
				if (Item)
				{
					UE_LOG(LogTemp,Warning,TEXT("SlotEquip::NativeOnMouse 아이템 캐스팅 성공 UnEquip함수호출 성공 "));
					Player->InvenComp->Server_UnEquip(Item);
				}
				else
				{
					UE_LOG(LogTemp,Warning,TEXT("SlotEquip::NativeOnMouse UnEquip함수호출 실패 "));
				}
			}
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
	if (Pc->GetPawn()->IsLocallyControlled())
	{  UE_LOG(LogTemp,Warning,TEXT("SlotEquip::NativeOnDrop: 플레이어 캐스팅 성공 이름은? %s"),*Pc->Player->GetName());
		ATestPlayer* Player = Cast<ATestPlayer>(Pc->GetPawn());
		Player->InvenComp->Server_EquipItem(ItemObject->ItemStruct,SlotType); }
	
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
