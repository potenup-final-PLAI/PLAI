// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/UI/MainBattleUI.h"

#include "Battle/UI/ReasonUI.h"
#include "Battle/Util/DebugHeader.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include "Enemy/BaseEnemy.h"
#include "Player/BattlePlayer.h"

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

void UMainBattleUI::AddOrder(const TArray<class ABaseBattlePawn*> unitArray)
{
	NET_PRINTLOG(TEXT("IN AddOrder"));
	// 시작할 때 원래 있던 UI 초기화
	HB_TurnOrder->ClearChildren();

	// unitArray에 있는 Unit을 Player인지 Enemy인지 확인해서 HB_TurnOrder에 AddChild
	for (const auto unit : unitArray)
	{
		if (auto* player = Cast<ABattlePlayer>(unit))
		{
			if (UUserWidget* playerOrderWidget = CreateWidget<UUserWidget>(GetWorld(), playerOrderFactory))
			{
				HB_TurnOrder->AddChild(playerOrderWidget);
			}
		}
		else if (auto* enemy = Cast<ABaseEnemy>(unit))
		{
			if (UUserWidget* enemyOrderWidget = CreateWidget<UUserWidget>(GetWorld(), enemyOrderFactory))
			{
				HB_TurnOrder->AddChild(enemyOrderWidget);
			}
		}
	}
}
