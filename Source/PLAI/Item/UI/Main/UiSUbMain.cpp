// Fill out your copyright notice in the Description page of Project Settings.


#include "UiSUbMain.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UUiSUbMain::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonSingle->OnClicked.AddDynamic(this,&UUiSUbMain::OnButtonSingle);
}

void UUiSUbMain::OnButtonSingle()
{
	UGameplayStatics::SetGamePaused(GetWorld(),false);
	RemoveFromParent();
}
