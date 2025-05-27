// Fill out your copyright notice in the Description page of Project Settings.


#include "UiTurn.h"

#include "Components/Button.h"

void UUiTurn::NativeConstruct()
{
	Super::NativeConstruct();

	Button_TurnStart->OnClicked.AddDynamic(this,&UUiTurn::OnTurnStart);
}

void UUiTurn::OnTurnStart()
{
}
