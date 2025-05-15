// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreInven.h"
#include "Components/WrapBox.h"
#include "PLAI/Item/UI/Slot/SlotStore.h"

void UStoreInven::NativeConstruct()
{
	Super::NativeConstruct();

	for (int32 i = 0; i < 32; i++)
	{
		USlotStore* SlotStore = CreateWidget<USlotStore>(GetWorld(), SlotStoreFactory);
		WrapBox->AddChild(SlotStore);
	}
}
