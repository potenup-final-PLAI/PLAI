// Fill out your copyright notice in the Description page of Project Settings.


#include "UIChaMain.h"

#include "UiMain.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "PLAI/Item/Login/LoginComp.h"

void UUIChaMain::NativeConstruct()
{
	Super::NativeConstruct();
	Button_Me->OnClicked.AddDynamic(this,&UUIChaMain::OnLoadMeInfo);
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
	Exp->SetPercent(UserFullInfo->character_info.current_exp/UserFullInfo->character_info.max_exp);
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
}

