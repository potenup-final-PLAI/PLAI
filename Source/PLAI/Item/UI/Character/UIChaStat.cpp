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
	int32 EHp = 0;
	int32 EAtk = 0;
	int32 EDef = 0;
	int32 ERes = 0;
	int32 ECri = 0;
	int32 ECriD = 0;
	int32 EMov = 0;
	int32 ESpd = 0;
	
    for (int i = 0; i < UserFullInfo->equipment_info.item_list.Num(); i++)
    {
    	EHp += UserFullInfo->equipment_info.item_list[i].options.hp;
    	EAtk += UserFullInfo->equipment_info.item_list[i].options.attack;
		EDef += UserFullInfo->equipment_info.item_list[i].options.defense;
		ERes += UserFullInfo->equipment_info.item_list[i].options.resistance;
		ECri += UserFullInfo->equipment_info.item_list[i].options.critical_rate;
		ECriD+= UserFullInfo->equipment_info.item_list[i].options.critical_damage;
		EMov += UserFullInfo->equipment_info.item_list[i].options.move_range;
		ESpd += UserFullInfo->equipment_info.item_list[i].options.speed;
    }
	HpE->SetText(FText::AsNumber(EHp));
	AtkE->SetText(FText::AsNumber(EAtk));
	DefE->SetText(FText::AsNumber(EDef));
	ResE->SetText(FText::AsNumber(ERes));
	CriE->SetText(FText::AsNumber(ECri));
	CriDE->SetText(FText::AsNumber(ECriD));
	SpdE->SetText(FText::AsNumber(ESpd));
	MovE->SetText(FText::AsNumber(EMov));
	
	int32 THp = 0;
	int32 TAtk = 0;
	int32 TDef = 0;
	int32 TRes = 0;
	int32 TCri = 0;
	int32 TCriD = 0;
	int32 TMov = 0;
	int32 TSpd = 0;
	
    for (int i = 0; i < UserFullInfo->character_info.traits.Num(); i++)
    {
    	UTextBlock* TraitName = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    	TraitName->SetText(FText::FromString(FString::Printf(TEXT("%s"), *UserFullInfo->character_info.traits[i])));
    	TraitName->SetFont(FSlateFontInfo(TraitName->GetFont().FontObject, 16));
    	UGridSlot* GridSlot = TraitsBox->AddChildToGrid(TraitName);
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
    			TraitStat->SetFont(FSlateFontInfo(TraitStat->GetFont().FontObject,11));
    			UGridSlot* GridSlotStat = TraitsBox->AddChildToGrid(TraitStat);
    			GridSlotStat->SetRow(i);
    			GridSlotStat->SetRowSpan(1);
    			GridSlotStat->SetColumn(2);
    			GridSlotStat->SetColumnSpan(2);

    			THp += TraitStruct->Hp;
    			TAtk += TraitStruct->Atk;
    			TDef += TraitStruct->Def;
    			TRes += 0;
    			TCri += TraitStruct->Crit;
    			TCriD += 0;
    			TSpd += TraitStruct->Spd;
    			TMov += TraitStruct->Mov;
    		}
    	}
    	HpT->SetText(FText::FromString(FString::Printf(TEXT("%i%%"), THp)));
    	AtkT->SetText(FText::FromString(FString::Printf(TEXT("%i%%"), TAtk)));
    	DefT->SetText(FText::FromString(FString::Printf(TEXT("%i%%"), TDef)));
    	ResT->SetText(FText::FromString(FString::Printf(TEXT("%i%%"), TRes)));
    	CriT->SetText(FText::FromString(FString::Printf(TEXT("%i%%"), TCri)));
    	CriDT->SetText(FText::FromString(FString::Printf(TEXT("%i%%"), TCri)));
    	SpdT->SetText(FText::FromString(FString::Printf(TEXT("%i%%"), TSpd)));
    	MovT->SetText(FText::FromString(FString::Printf(TEXT("%i%%"), TMov)));
    	
    	UTextBlock* TraitStat = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    	TraitName->SetText(FText::FromString(FString::Printf(TEXT("%s"), *UserFullInfo->character_info.traits[i])));
    }

	HpTot->SetText(FText::AsNumber((UserFullInfo->character_info.stats.hp+EHp) * (1.0f + THp/100.0f) ));
	AtkTot->SetText(FText::AsNumber((UserFullInfo->character_info.stats.attack+EAtk) * TAtk));
	DefTot->SetText(FText::AsNumber((UserFullInfo->character_info.stats.defense+EDef) * TDef));
	ResTot->SetText(FText::AsNumber((UserFullInfo->character_info.stats.resistance+ERes) * TRes));
	CriTot->SetText(FText::AsNumber((UserFullInfo->character_info.stats.critical_rate+ECri) * TCri));
	CriDTot->SetText(FText::AsNumber((UserFullInfo->character_info.stats.critical_damage+ECriD) * TCriD));
	SpdTot->SetText(FText::AsNumber((UserFullInfo->character_info.stats.speed + ESpd) * TSpd));
	MovTot->SetText(FText::AsNumber((UserFullInfo->character_info.stats.move_range+EMov) * TMov));
	
	Name->SetText(FText::FromString(UserFullInfo->character_info.character_name));
	Job->SetText(FText::FromString(UserFullInfo->character_info.job));
	Gen->SetText(FText::FromString(UserFullInfo->character_info.gender));

	// Traits->SetText(FText::AsNumber(UserFullInfo->character_info.traits[0].
	
	Level->SetText(FText::AsNumber(UserFullInfo->character_info.level));
	// HpSco->SetText(FText::AsNumber(UserFullInfo->character_info.stats.hp));

	HP->SetText(FText::AsNumber(UserFullInfo->character_info.stats.hp));
	Atk->SetText(FText::AsNumber(UserFullInfo->character_info.stats.attack));
	Def->SetText(FText::AsNumber(UserFullInfo->character_info.stats.defense));
	Res->SetText(FText::AsNumber(UserFullInfo->character_info.stats.resistance));
	Cri->SetText(FText::AsNumber(UserFullInfo->character_info.stats.critical_rate));
	CriD->SetText(FText::AsNumber(UserFullInfo->character_info.stats.critical_damage));
	Spd->SetText(FText::AsNumber(UserFullInfo->character_info.stats.speed));
	Mov->SetText(FText::AsNumber(UserFullInfo->character_info.stats.move_range));

	if (UserFullInfo->character_info.current_exp > 0 && UserFullInfo->character_info.max_exp > 0)
	{
		ExpBar->SetPercent(UserFullInfo->character_info.current_exp/UserFullInfo->character_info.max_exp);
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("UicahMain SetUiChaStat 경험치 안들어옴"))
	}
}

// HpTot->SetText(FText::AsNumber((UserFullInfo->character_info.stats.hp+EHp) * (THp/100) + 1) );
// AtkTot->SetText(FText::AsNumber((UserFullInfo->character_info.stats.attack+EAtk) * (TAtk/100) + 1) );
// DefTot->SetText(FText::AsNumber((UserFullInfo->character_info.stats.defense+EDef) * (TDef/100) + 1) );
// ResTot->SetText(FText::AsNumber((UserFullInfo->character_info.stats.resistance+ERes) * (TRes/100) + 1) );
// CriTot->SetText(FText::AsNumber((UserFullInfo->character_info.stats.critical_rate+ECri) * (TCri/100) + 1) );
// CriDTot->SetText(FText::AsNumber((UserFullInfo->character_info.stats.critical_damage+ECriD) * (TCriD/100) + 1) );
// SpdTot->SetText(FText::AsNumber((UserFullInfo->character_info.stats.speed + ESpd) * (TSpd/100) + 1) );
// MovTot->SetText(FText::AsNumber((UserFullInfo->character_info.stats.move_range+EMov) * (TMov/100) + 1) );