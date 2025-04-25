// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickInven.h"

#include "Components/HorizontalBox.h"

void UQuickInven::NativeConstruct()
{
	Super::NativeConstruct();
	for (int i = 0; i < 12 ; i++)
	{
		QuickSlot = CreateWidget<USlotQuick>(GetWorld(),QuickSlotFactory);
		QuickSlotBox->AddChild(QuickSlot);
	}
}
