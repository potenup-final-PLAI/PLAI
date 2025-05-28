// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreInven.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/WrapBox.h"
#include "PLAI/Item/UI/Slot/SlotStore.h"

void UStoreInven::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Equip->OnClicked.AddDynamic(this,&UStoreInven::OnButtonEquip);
	Button_Consume->OnClicked.AddDynamic(this,&UStoreInven::OnButtonConsume);
	Button_Legendary->OnClicked.AddDynamic(this,&UStoreInven::OnButtonLegendary);

	Equip->SetVisibility(ESlateVisibility::Visible);
	Consume->SetVisibility(ESlateVisibility::Hidden);
	Legendary->SetVisibility(ESlateVisibility::Hidden);

	TArray<FName>RawNames = ItemTable->GetRowNames();
	for (FName RawName : RawNames)
	{
		FItemStructTable* ItemStructTable = ItemTable->FindRow<FItemStructTable>(RawName,"StoreInven");
		if (ItemStructTable->ItemTop == 1 && ItemStructTable->ItemIndexDetail !=3)
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
		else if (ItemStructTable->ItemTop == 3 || ItemStructTable->ItemIndexDetail == 3)
		{
			USlotStore* SlotStore = CreateWidget<USlotStore>(GetWorld(), SlotStoreFactory);
			SlotStore->ItemStructTable = *ItemStructTable;
			LegendaryBox->AddChild(SlotStore);
			SlotStore->SlotImageUpdate();
		}
	}
	
}

void UStoreInven::OnButtonEquip()
{
	Equip->SetVisibility(ESlateVisibility::Visible);
	Consume->SetVisibility(ESlateVisibility::Hidden);
	Legendary->SetVisibility(ESlateVisibility::Hidden);
}

void UStoreInven::OnButtonConsume()
{
	Equip->SetVisibility(ESlateVisibility::Hidden);
	Consume->SetVisibility(ESlateVisibility::Visible);
	Legendary->SetVisibility(ESlateVisibility::Hidden);
}

void UStoreInven::OnButtonLegendary()
{
	Equip->SetVisibility(ESlateVisibility::Hidden);
	Consume->SetVisibility(ESlateVisibility::Hidden);
	Legendary->SetVisibility(ESlateVisibility::Visible);
}
