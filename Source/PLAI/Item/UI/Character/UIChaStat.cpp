// Fill out your copyright notice in the Description page of Project Settings.


#include "UIChaStat.h"

#include "Blueprint/WidgetTree.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/ProgressBar.h"
#include "PLAI/Item/Login/UserStruct.h"
#include "Components/TextBlock.h"
#include "PLAI/Item/TestPlayer/TraitStructTable/TraitStructTable.h"

void UUIChaStat::SetUiChaStat(FUserFullInfo* UserFullInfo)
{
    for (int i = 0; i < UserFullInfo->character_info.traits.Num(); i++)
    {
    	UTextBlock* TraitName = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    	TraitName->SetText(FText::FromString(FString::Printf(TEXT("%s"), *UserFullInfo->character_info.traits[i])));
    	UGridSlot* GridSlot = TraitsBox->AddChildToGrid(TraitName);
    	TraitName->SetFont(FSlateFontInfo(TraitName->GetFont().FontObject, 18));
    	GridSlot->SetRow(i);
	    GridSlot->SetRowSpan(1);
    	GridSlot->SetColumn(1);
    	GridSlot->SetColumnSpan(1);

    	TArray<FName>Traits = TraitStructTable->GetRowNames();
    	for (FName Trait : Traits)
    	{
    		FTraitStructTable* TraitStruct = TraitStructTable->FindRow<FTraitStructTable>(Trait,TEXT("UiChaStat"));
    		if (TraitStruct->Trait == UserFullInfo->character_info.traits[i])
    		{
    			UTextBlock* TraitStat = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    			TraitStat->SetText(FText::FromString(TraitStruct->TraitStat));
    			TraitStat->SetFont(FSlateFontInfo(TraitStat->GetFont().FontObject,12));
    			UGridSlot* GridSlotStat = TraitsBox->AddChildToGrid(TraitName);
    			GridSlotStat->SetRow(i);
    			GridSlotStat->SetRowSpan(1);
    			GridSlotStat->SetColumn(2);
    			GridSlotStat->SetColumnSpan(2);
    		}
    	}
    	
    	UTextBlock* TraitStat = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    	TraitName->SetText(FText::FromString(FString::Printf(TEXT("%s"), *UserFullInfo->character_info.traits[i])));
    }
	
	Name->SetText(FText::FromString(UserFullInfo->character_info.character_name));
	Job->SetText(FText::FromString(UserFullInfo->character_info.job));
	Gen->SetText(FText::FromString(UserFullInfo->character_info.gender));

	// Traits->SetText(FText::AsNumber(UserFullInfo->character_info.traits[0].
	
	Level->SetText(FText::AsNumber(UserFullInfo->character_info.level));
	// HpSco->SetText(FText::AsNumber(UserFullInfo->character_info.stats.hp));

	Atk->SetText(FText::AsNumber(UserFullInfo->character_info.stats.attack));
	Def->SetText(FText::AsNumber(UserFullInfo->character_info.stats.defense));
	Res->SetText(FText::AsNumber(UserFullInfo->character_info.stats.resistance));
	Cri->SetText(FText::AsNumber(UserFullInfo->character_info.stats.critical_rate));
	CriD->SetText(FText::AsNumber(UserFullInfo->character_info.stats.critical_damage));
	Spe->SetText(FText::AsNumber(UserFullInfo->character_info.stats.speed));

	if (UserFullInfo->character_info.current_exp > 0 && UserFullInfo->character_info.max_exp > 0)
	{
		ExpBar->SetPercent(UserFullInfo->character_info.current_exp/UserFullInfo->character_info.max_exp);
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("UicahMain SetUiChaStat 경험치 안들어옴"))
	}
}
