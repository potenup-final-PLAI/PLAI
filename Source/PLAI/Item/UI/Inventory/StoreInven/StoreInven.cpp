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

	for (int32 i = 0; i < 34; i++)
	{
		USlotStore* SlotStore = CreateWidget<USlotStore>(GetWorld(), SlotStoreFactory);
		WrapBox->AddChild(SlotStore);
	}
}

void UStoreInven::OnButtonEquip()
{
}

void UStoreInven::OnButtonConsume()
{
}

void UStoreInven::OnButtonLegendary()
{
}
