// Fill out your copyright notice in the Description page of Project Settings.

#include "UiPortal.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "PLAI/Item/Portal/Warp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"

void UUiPortal::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button_Village->OnClicked.AddDynamic(this,&UUiPortal::OnButton_Village);
	Button_Mountain->OnClicked.AddDynamic(this,&UUiPortal::OnButton_Mountain);
	Button_Dessert->OnClicked.AddDynamic(this,&UUiPortal::OnButton_Dessert);
	Button_Cave->OnClicked.AddDynamic(this,&UUiPortal::OnButton_Cave);
	Button_OpenMap->OnClicked.AddDynamic(this,&UUiPortal::OnButton_OpenMap);
	
}

void UUiPortal::OnButton_Village()
{
	Warp->WarpLevel(TestPlayer,0);
}

void UUiPortal::OnButton_Mountain()
{
	Warp->WarpLevel(TestPlayer,1);
}

void UUiPortal::OnButton_Dessert()
{
	Warp->WarpLevel(TestPlayer,2);
}

void UUiPortal::OnButton_Cave()
{
	Warp->WarpLevel(TestPlayer,3);
}

void UUiPortal::OnButton_OpenMap()
{
	if (bOpenMap == false)
	{
		WorldMap->SetVisibility(ESlateVisibility::Visible);
		bOpenMap = true;
	}
	else
	{
		WorldMap->SetVisibility(ESlateVisibility::Hidden);
		bOpenMap = false;
	}
}
