// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/UI/BattlePlayerInfoUI.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Player/BattlePlayer.h"


void UBattlePlayerInfoUI::SetPlayerName(const FString& unitName)
{
	txt_PlayerName->SetText(FText::FromString(unitName));
}

void UBattlePlayerInfoUI::SetPlayerHPUI(class ABaseBattlePawn* unit)
{
	if (ABattlePlayer* player = Cast<ABattlePlayer>(unit))
	{
		// MaxHP 세팅
		txt_PlayerMaxHP->SetText(FText::AsNumber(player->maxHP));
		
		// CurHP 세팅
		int32 curHP = player->hp;
		txt_PlayerHP->SetText(FText::AsNumber(curHP));
		UE_LOG(LogTemp, Warning, TEXT("SetPlayerHPUI Player curHP : %d / maxHP : %d"), curHP, player->maxHP);
	}
}

void UBattlePlayerInfoUI::PlayerUpdateHP(class ABaseBattlePawn* unit, int32 hp)
{
	if (PGB_PlayerHP)
	{
		// BattlePlayerInfo UI 업데이트
		if (auto* player = Cast<ABattlePlayer>(unit)) // Warning뜸
		{
			if (player->maxHP <= 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("Warning: maxHP is zero or less in PlayerUpdateHP"));
				return; // 또는 SetPercent(0.f)
			}
			
			txt_PlayerHP->SetText(FText::AsNumber(hp));
			
			// 현재 HP를 0~1로 만들어서 퍼센트 업데이트
			float hpPercent = static_cast<float>(hp) / static_cast<float>(player->maxHP);
			PGB_PlayerHP->SetPercent(hpPercent);
		}
	}
}
