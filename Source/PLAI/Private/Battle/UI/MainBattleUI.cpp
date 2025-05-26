// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/UI/MainBattleUI.h"

#include "Battle/UI/ReasonUI.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"

void UMainBattleUI::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UMainBattleUI::AddAP()
{
	UUserWidget* apUI = CreateWidget(GetWorld(), apFactory);
	if (HB_AP && HB_AP->GetChildrenCount() < 5) HB_AP->AddChildToHorizontalBox(apUI);
}

void UMainBattleUI::AddReason(FString dialogue)
{
	UE_LOG(LogTemp, Warning, TEXT("IN AddReason"));
	if (UReasonUI* reason = CreateWidget<UReasonUI>(GetWorld(), reasonFactory))
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast Reason"));
		reason->SetText(dialogue);
		SB_Reason->AddChild(reason);
	}
}
