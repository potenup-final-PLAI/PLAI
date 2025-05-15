// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemInven.h"

#include "ItemGold.h"
#include "Components/Image.h"
#include "Components/WrapBox.h"

void UItemInven::NativeConstruct()
{
	Super::NativeConstruct();
	
	for (int32 i = 0; i < 32; i++)
	{
		SlotUi = CreateWidget<USlot>(GetWorld(),SlotFactory);
		FSlateBrush Brush;
		
		WrapBox->AddChild(SlotUi);
	}
}
