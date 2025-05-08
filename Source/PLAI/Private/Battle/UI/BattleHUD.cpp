// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/UI/BattleHUD.h"

#include "Battle/UI/MainBattleUI.h"

void ABattleHUD::BeginPlay()
{
	Super::BeginPlay();

	if (mainUIFactory)
	{
		mainUI = CreateWidget<UMainBattleUI>(GetWorld(), mainUIFactory);
		if (mainUI)
		{
			mainUI->AddToViewport();
		}
	}
}
