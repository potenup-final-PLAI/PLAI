// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipInven.h"

#include "Blueprint/DragDropOperation.h"
#include "Components/VerticalBox.h"
#include "PLAI/Item/Item/ItemObject.h"

void UEquipInven::NativeConstruct()
{
	Super::NativeConstruct();
	TArray<EquipSlotType> SlotTypes = {
		EquipSlotType::Weapon,
		EquipSlotType::Armor,
		EquipSlotType::Helmet,
		EquipSlotType::Gloves,
		EquipSlotType::Boots,
	};

	for (int32 i = 0; i < 5; i++)
	{
		SlotEquip = CreateWidget<USlotEquip>(GetWorld(),SlotEquipFactory);
		SlotEquip->SlotType = SlotTypes[i];
		SlotEquip->EquipInven = this;
		LeftBox->AddChild(SlotEquip);
	}

	for (int32 i = 0; i < 5; i++)
	{
		SlotEquip = CreateWidget<USlotEquip>(GetWorld(),SlotEquipFactory);
		RightBox->AddChild(SlotEquip);
	}
}

bool UEquipInven::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp,Warning,TEXT("EquipInven::NativeOnDrop"));
	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
	if (ItemObject->ItemStruct.ItemTop == 1)
	{
		for (UWidget* Widget : LeftBox->GetAllChildren())
		{
			// USlotEquip* ESlot = Cast<USlotEquip>(Widget);
			// if (ESlot->SlotType == EquipSlotType::Weapon && ItemObject->ItemStruct.ItemIndex == 0)
			// {
			// 	ESlot->ItemStruct = ItemObject->ItemStruct;
			// 	ESlot->SlotImageUpdate();
			// }
			// if (ESlot->SlotType == EquipSlotType::Armor && ItemObject->ItemStruct.ItemIndex == 1)
			// {
			// 	ESlot->ItemStruct = ItemObject->ItemStruct;
			// 	ESlot->SlotImageUpdate();
			// }
		}
	}
	
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
