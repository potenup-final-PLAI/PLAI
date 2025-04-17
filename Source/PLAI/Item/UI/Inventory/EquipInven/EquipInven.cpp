// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipInven.h"

#include "Components/VerticalBox.h"

void UEquipInven::NativeConstruct()
{
	Super::NativeConstruct();

	for (int32 i = 0; i < 5; i++)
	{
		SlotEquip = CreateWidget<USlotEquip>(GetWorld(),SlotEquipFactory);
		LeftBox->AddChild(SlotEquip);

		SlotEquip = CreateWidget<USlotEquip>(GetWorld(),SlotEquipFactory);
		RightBox->AddChild(SlotEquip);
	}

}
