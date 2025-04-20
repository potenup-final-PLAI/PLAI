// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreInven.h"
#include "Components/WrapBox.h"
#include "PLAI/Item/UI/Slot/SlotStore.h"

void UStoreInven::NativeConstruct()
{
	Super::NativeConstruct();

	if (SlotStoreFactory)
	{
		UE_LOG(LogTemp, Error, TEXT("UStoreInve::BeginPlay 스토슬롯 생성"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UStoreInve::BeginPlay 스토슬롯 생성불가"));
	}

	for (int32 i = 0; i < 24; i++)
	{
		USlotStore* SlotStore = CreateWidget<USlotStore>(GetWorld(), SlotStoreFactory);
		WrapBox->AddChild(SlotStore);
	}
}
