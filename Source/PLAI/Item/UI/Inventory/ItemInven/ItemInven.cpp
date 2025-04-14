// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemInven.h"

#include "Components/Image.h"
#include "Components/WrapBox.h"

void UItemInven::NativeConstruct()
{
	Super::NativeConstruct();

	for (int32 i = 0; i < 24; i++)
	{
		SlotUi = CreateWidget<USlot>(GetWorld(),SlotFactory);
		FSlateBrush Brush = FSlateBrush();
		SlotUi->SlotImage->SetBrush(Brush);
		WrapBox->AddChild(SlotUi);
	}
}
