// Fill out your copyright notice in the Description page of Project Settings.


#include "UIChaMain.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UUIChaMain::SetUiChaStat(FUserFullInfo* UserFullInfo)
{
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
