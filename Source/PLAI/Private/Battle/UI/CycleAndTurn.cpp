// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/UI/CycleAndTurn.h"

#include "Components/TextBlock.h"

void UCycleAndTurn::SetCycleText(int32 cycle)
{
	txt_Cycle->SetText(FText::AsNumber(cycle));
}

void UCycleAndTurn::SetTurnText(FString unit)
{
	txt_turn->SetText(FText::FromString(unit));
}
