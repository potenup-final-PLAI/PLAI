// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemInven.h"

#include "Components/WrapBox.h"

void UItemInven::NativeConstruct()
{
	Super::NativeConstruct();

	for (int32 i = 0; i < 24; i++)
	{
		SlotUi = CreateWidget<USlot>(GetWorld(),SlotFactory);
		WrapBox->AddChild(SlotUi);
	}
}
