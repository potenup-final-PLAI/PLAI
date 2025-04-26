// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickInven.h"

#include "Components/HorizontalBox.h"
#include "Components/Image.h"

void UQuickInven::NativeConstruct()
{
	Super::NativeConstruct();
	for (int i = 0; i < 12 ; i++)
	{
		QuickSlot = CreateWidget<USlotQuick>(GetWorld(),QuickSlotFactory);
		FSlateBrush Brush;
		Brush.SetResourceObject(nullptr);
		Brush.DrawAs = ESlateBrushDrawType::NoDrawType;
		QuickSlot->SlotImage->SetBrush(Brush);
		
		QuickSlotBox->AddChild(QuickSlot);
	}
}
