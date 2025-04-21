// Fill out your copyright notice in the Description page of Project Settings.


#include "UiMain.h"

#include "Components/Button.h"

void UUiMain::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonStart->OnClicked.AddDynamic(this,&UUiMain::RemoveFromParent);
}
