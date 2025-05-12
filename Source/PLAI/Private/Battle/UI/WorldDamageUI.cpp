// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/UI/WorldDamageUI.h"

#include "Components/TextBlock.h"

void UWorldDamageUI::SetDamageText(const int32 damage)
{
	if (txt_Damage)
	{
		txt_Damage->SetText(FText::AsNumber(damage));
	}
}
