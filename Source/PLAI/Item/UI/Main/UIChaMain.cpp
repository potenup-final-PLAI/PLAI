// Fill out your copyright notice in the Description page of Project Settings.


#include "UIChaMain.h"

#include "UiMain.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/TextBlock.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/Login/LoginComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Inventory/EquipInven/EquipInven.h"
#include "PLAI/Item/UI/Inventory/ItemDetail/ItemDetail.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemInven.h"
#include "PLAI/Item/UI/Inventory/QuickInven/QuickInven.h"
#include "PLAI/Item/UI/Slot/SlotCre.h"

void UUIChaMain::NativeConstruct()
{
	Super::NativeConstruct();
	Button_Me->OnClicked.AddDynamic(this,&UUIChaMain::OnLoadMeInfo);
	Button_SelectMode->OnClicked.AddDynamic(this,&UUIChaMain::UUIChaMain::OnSelectMode);
}

void UUIChaMain::SetUiChaStat(FUserFullInfo* UserFullInfo)
{
    if (!UserFullInfo)
    {
    	UE_LOG(LogTemp,Warning,TEXT("UiChaMain UserFUllinfo 아직 안들어옴여"))
	    return;
    }
	
	Name->SetText(FText::FromString(UserFullInfo->character_info.character_name));
	Job->SetText(FText::FromString(UserFullInfo->character_info.job));
	Gen->SetText(FText::FromString(UserFullInfo->character_info.gender));

	// Traits->SetText(FText::AsNumber(UserFullInfo->character_info.traits[0].
	
	Level->SetText(FText::AsNumber(UserFullInfo->character_info.level));
	HpSco->SetText(FText::AsNumber(UserFullInfo->character_info.stats.hp));

	Atk->SetText(FText::AsNumber(UserFullInfo->character_info.stats.attack));
	Def->SetText(FText::AsNumber(UserFullInfo->character_info.stats.defense));
	Res->SetText(FText::AsNumber(UserFullInfo->character_info.stats.resistance));
	Cri->SetText(FText::AsNumber(UserFullInfo->character_info.stats.critical_rate));
	Crid->SetText(FText::AsNumber(UserFullInfo->character_info.stats.critical_damage));
	Spe->SetText(FText::AsNumber(UserFullInfo->character_info.stats.speed));

	Gold->SetText(FText::AsNumber(UserFullInfo->inventory_info.gold));
	if (UserFullInfo->character_info.current_exp > 0 && UserFullInfo->character_info.max_exp > 0)
	{
		Exp->SetPercent(UserFullInfo->character_info.current_exp/UserFullInfo->character_info.max_exp);
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("UicahMain SetUiChaStat 경험치 안들어옴"))
	}
}

void UUIChaMain::OnLoadMeInfo()
{
	UE_LOG(LogTemp,Warning,TEXT("UiChaMain::OnLoadMeInfo 버튼 누름 "));
	if (!UiMain)
	{
		UE_LOG(LogTemp,Warning,TEXT("UUiChaMain에서 UiMain -> UUIChaMain 포인터 넘겨주기 실패 "))
		return;
	}
	UiMain->LoginComp->HttpMePost();
	UiMain->LoginComp->TestPlayer->InvenComp->MenuInven->AddToViewport(1);
	
	// 퀵슬롯 디테일창은 끄자
	UiMain->LoginComp->TestPlayer->InvenComp->MenuInven->WBP_QuickInven->SetVisibility(ESlateVisibility::Hidden);
    UiMain->LoginComp->TestPlayer->InvenComp->MenuInven->WBP_ItemDetail->SetVisibility(ESlateVisibility::Hidden);
	UiMain->LoginComp->TestPlayer->InvenComp->MenuInven->WBP_SlotCre->SetVisibility(ESlateVisibility::Hidden);
	// 퀵슬롯 아이템 장비창 들어온느거 확인시켜주자
	UiMain->LoginComp->TestPlayer->InvenComp->MenuInven->WBP_EquipInven->SetVisibility(ESlateVisibility::Visible);
	UiMain->LoginComp->TestPlayer->InvenComp->MenuInven->WBP_ItemInven->SetVisibility(ESlateVisibility::Visible);
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
	{
		UiMain->LoginComp->TestPlayer->InvenComp->MenuInven->WBP_EquipInven->SetVisibility(ESlateVisibility::Hidden);
	    UiMain->LoginComp->TestPlayer->InvenComp->MenuInven->WBP_ItemInven->SetVisibility(ESlateVisibility::Hidden);
	},2.0f,false);
}

void UUIChaMain::OnSelectMode()
{
	UiMain->LoginComp->TestPlayer->CaptureComp->DestroyComponent();
	RemoveFromParent();
}

