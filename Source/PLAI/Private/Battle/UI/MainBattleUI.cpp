// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/UI/MainBattleUI.h"

#include "Components/HorizontalBox.h"

void UMainBattleUI::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UMainBattleUI::AddAP()
{
	// 총알 위젯을 만들고
	UUserWidget* apUI = CreateWidget(GetWorld(), apFactory);
	// 만들어진 총알 위젯을 패널에 자식으로 붙이자
	HB_AP->AddChildToHorizontalBox(apUI);
}
