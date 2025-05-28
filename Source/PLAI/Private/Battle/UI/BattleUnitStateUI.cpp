// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/UI/BattleUnitStateUI.h"

#include "BaseBattlePawn.h"
#include "Battle/TurnSystem/TurnManager.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Components/WidgetSwitcher.h"
#include "Enemy/BaseEnemy.h"
#include "Player/BattlePlayer.h"
#include "Engine/NetDriver.h"

void UBattleUnitStateUI::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UBattleUnitStateUI::ShowBaseUI()
{
	if (WS_BattleUnitState) WS_BattleUnitState->SetActiveWidgetIndex(0);
}


void UBattleUnitStateUI::SetUnitName(const FString& unitName)
{
	txt_UnitName->SetText(FText::FromString(unitName));
}

void UBattleUnitStateUI::ShowHoverUI()
{
	if (WS_BattleUnitState) WS_BattleUnitState->SetActiveWidgetIndex(1);
}

void UBattleUnitStateUI::SetHPUI(ABaseBattlePawn* unit)
{
	UE_LOG(LogTemp, Warning, TEXT("NetMode: %s"), *GetNetModeString(GetWorld()));
	if (ABattlePlayer* player = Cast<ABattlePlayer>(unit))
	{
		txt_HP->SetText(FText::AsNumber(player->maxHP));
		UE_LOG(LogTemp, Warning, TEXT("SetHPUI Player maxHP = %d"), player->maxHP);
	}
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(unit))
	{
		if (enemy->enemybattleState)
		{
			enemy->maxHP = enemy->enemybattleState->hp;
			txt_HP->SetText(FText::AsNumber(enemy->maxHP));
			UE_LOG(LogTemp, Warning, TEXT("SetHPUI Enemy maxHP = %d"), enemy->maxHP);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SetHPUI failed: unit is neither Player nor Enemy"));
	}
}

void UBattleUnitStateUI::UpdateHP(int32 hp, ABaseBattlePawn* unit)
{
	// 기본 HP
	if (PGB_BaseHP)
	{
		if (auto* player = Cast<ABattlePlayer>(unit))
		{
			// 현재 HP를 0~1로 만들어서 퍼센트 업데이트
			float hpPercent = static_cast<float>(hp) / static_cast<float>(player->maxHP);
			PGB_BaseHP->SetPercent(hpPercent);
			UE_LOG(LogTemp, Warning, TEXT("UpdateHP: hp = %d / maxHP = %d / percent = %f"), hp, player->maxHP, hpPercent);
		}
		else if (auto* enemy = Cast<ABaseEnemy>(unit))
		{
			// 현재 HP를 0~1로 만들어서 퍼센트 업데이트
			float hpPercent = static_cast<float>(hp) / static_cast<float>(enemy->maxHP);
			PGB_BaseHP->SetPercent(hpPercent);
		}
	}
	// 호버 시
	if (txt_HP && PGB_HP)
	{
		if (auto* player = Cast<ABattlePlayer>(unit))
		{
			// 현재 HP를 받아서 UI에 업데이트
			txt_HP->SetText(FText::AsNumber(hp));
			// 현재 HP를 0~1로 만들어서 퍼센트 업데이트
			float hpPercent = static_cast<float>(hp) / static_cast<float>(player->maxHP);
			PGB_HP->SetPercent(hpPercent);
			UE_LOG(LogTemp, Warning, TEXT("UpdateHP: hp = %d / maxHP = %d / percent = %f"), hp, player->maxHP, hpPercent);
		}
		else if (auto* enemy = Cast<ABaseEnemy>(unit))
		{
			// 현재 HP를 받아서 UI에 업데이트
			txt_HP->SetText(FText::AsNumber(hp));
			// 현재 HP를 0~1로 만들어서 퍼센트 업데이트
			float hpPercent = static_cast<float>(hp) / static_cast<float>(enemy->maxHP);
			PGB_HP->SetPercent(hpPercent);
			UE_LOG(LogTemp, Warning, TEXT("UpdateHP: hp = %d / maxHP = %d / percent = %f"), hp, enemy->maxHP, hpPercent);
		}

	}
}

void UBattleUnitStateUI::ShowPrintSkillNameUI()
{
	if (WS_BattleUnitState) WS_BattleUnitState->SetActiveWidgetIndex(2);
}

void UBattleUnitStateUI::SetPrintSkillName(const FString& skillName)
{
	txt_PrintSkillName->SetText(FText::FromString(skillName));
}

void UBattleUnitStateUI::ShowAPIReasonUI()
{
	if (WS_BattleUnitState) WS_BattleUnitState->SetActiveWidgetIndex(3);
}

void UBattleUnitStateUI::SetAPIReason(const FString& dialogue)
{
	TB_APIReason->SetText(FText::FromString(dialogue));
}

FString UBattleUnitStateUI::GetNetModeString(UWorld* World)
{
	switch (World->GetNetMode())
	{
	case NM_Standalone: return TEXT("Standalone");
	case NM_Client: return TEXT("Client");
	case NM_ListenServer: return TEXT("ListenServer");
	case NM_DedicatedServer: return TEXT("DedicatedServer");
	default: return TEXT("Unknown");
	}
}

void UBattleUnitStateUI::UpdatePlayerHPUI(int32 hp)
{
	// HP가 같다면 return
	if (curHP == hp) return;
	// 아니라면 curHP 업데이트
	curHP = hp;
	// UI 변경
	txt_HP->SetText(FText::AsNumber(hp));
	UE_LOG(LogTemp, Warning, TEXT("SetHPUI Player maxHP = %d"), hp);
}

void UBattleUnitStateUI::UpdateEnemyHPUI(int32 hp)
{
	txt_HP->SetText(FText::AsNumber(hp));
}
