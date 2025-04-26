// Fill out your copyright notice in the Description page of Project Settings.

#include "UiPortal.h"

#include "Components/Button.h"
#include "PLAI/Item/Portal/Warp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"

void UUiPortal::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button_Village->OnClicked.AddDynamic(this,&UUiPortal::OnButton_Village);
	Button_Mountain->OnClicked.AddDynamic(this,&UUiPortal::OnButton_Mountain);
	Button_Dessert->OnClicked.AddDynamic(this,&UUiPortal::OnButton_Dessert);
}

void UUiPortal::OnButton_Village()
{
	Warp->WarpLevel(Warp->WarpPlayer,0);
}

void UUiPortal::OnButton_Mountain()
{
	Warp->WarpLevel(Warp->WarpPlayer,1);
}

void UUiPortal::OnButton_Dessert()
{
	Warp->WarpLevel(Warp->WarpPlayer,2);
}
