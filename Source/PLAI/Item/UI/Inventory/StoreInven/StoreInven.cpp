// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreInven.h"

#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "PLAI/Item/UI/Slot/SlotStore.h"

void UStoreInven::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Equip->OnClicked.AddDynamic(this,&UStoreInven::OnButtonEquip);
	Button_Consume->OnClicked.AddDynamic(this,&UStoreInven::OnButtonConsume);
	Button_Legendary->OnClicked.AddDynamic(this,&UStoreInven::OnButtonLegendary);

	// EquipBox->SetVisibility(ESlateVisibility::Visible);
	// ConsumeBox->SetVisibility(ESlateVisibility::Hidden);
	// LegendaryBox->SetVisibility(ESlateVisibility::Hidden);

	TArray<FName>RawNames = ItemTable->GetRowNames();
	for (FName RawName : RawNames)
	{
		FItemStructTable* ItemStructTable = ItemTable->FindRow<FItemStructTable>(RawName,"StoreInven");
		if (ItemStructTable->ItemTop == 1)
		{
			USlotStore* SlotStore = CreateWidget<USlotStore>(GetWorld(), SlotStoreFactory);
			SlotStore->ItemStructTable = *ItemStructTable;
			EquipBox->AddChild(SlotStore);
			SlotStore->SlotImageUpdate();
		}
		else if (ItemStructTable->ItemTop == 0)
		{
			USlotStore* SlotStore = CreateWidget<USlotStore>(GetWorld(), SlotStoreFactory);
			SlotStore->ItemStructTable = *ItemStructTable;
			ConsumeBox->AddChild(SlotStore);
			SlotStore->SlotImageUpdate();
		}
		else if (ItemStructTable->ItemTop == 3,4)
		{
			USlotStore* SlotStore = CreateWidget<USlotStore>(GetWorld(), SlotStoreFactory);
			SlotStore->ItemStructTable = *ItemStructTable;
			LegendaryBox->AddChild(SlotStore);
			SlotStore->SlotImageUpdate();
		}
	}

	// for (int32 i = 0; i < 34; i++)
	// {
	// 	USlotStore* SlotStore = CreateWidget<USlotStore>(GetWorld(), SlotStoreFactory);
	// 	WrapBox->AddChild(SlotStore);
	// }
}

void UStoreInven::OnButtonEquip()
{
	EquipBox->SetVisibility(ESlateVisibility::Visible);
	ConsumeBox->SetVisibility(ESlateVisibility::Hidden);
	LegendaryBox->SetVisibility(ESlateVisibility::Hidden);
}

void UStoreInven::OnButtonConsume()
{
	EquipBox->SetVisibility(ESlateVisibility::Hidden);
	ConsumeBox->SetVisibility(ESlateVisibility::Visible);
	LegendaryBox->SetVisibility(ESlateVisibility::Hidden);
}

void UStoreInven::OnButtonLegendary()
{
	EquipBox->SetVisibility(ESlateVisibility::Hidden);
	ConsumeBox->SetVisibility(ESlateVisibility::Hidden);
	LegendaryBox->SetVisibility(ESlateVisibility::Visible);
}
