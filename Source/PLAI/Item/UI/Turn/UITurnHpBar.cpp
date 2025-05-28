// Fill out your copyright notice in the Description page of Project Settings.


#include "UITurnHpBar.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "PLAI/Item/Login/UserStruct.h"

void UUITurnHpBar::SetHpBar(FTurnPlayerStruct TurnPlayerStruct)
{
	CurrentHp->SetText(FText::AsNumber(TurnPlayerStruct.CurrentHp));
	MaxHp->SetText(FText::AsNumber(TurnPlayerStruct.MaxHp));

	HpBar->SetPercent(static_cast<float>(TurnPlayerStruct.CurrentHp)/static_cast<float>(TurnPlayerStruct.MaxHp));
}
