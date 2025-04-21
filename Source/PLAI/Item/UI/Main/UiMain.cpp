// Fill out your copyright notice in the Description page of Project Settings.


#include "UiMain.h"

#include "UiSign.h"
#include "Components/Button.h"

void UUiMain::SetUiSign()
{
	if (bUiSign == false)
	{
		WbpUiSign->SetVisibility(ESlateVisibility::Visible);
		bUiSign = true;
	}
	else
	{
		WbpUiSign->SetVisibility(ESlateVisibility::Hidden);
		bUiSign = false;
	}
}

void UUiMain::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonStart->OnClicked.AddDynamic(this,&UUiMain::RemoveFromParent);
	ButtonSignin->OnClicked.AddDynamic(this,&UUiMain::SetUiSign);
	
	WbpUiSign->SetVisibility(ESlateVisibility::Hidden);
}
