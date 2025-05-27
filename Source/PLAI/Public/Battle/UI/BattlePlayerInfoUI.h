// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattlePlayerInfoUI.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UBattlePlayerInfoUI : public UUserWidget
{
	GENERATED_BODY()

public:
	// Unit 이름 
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* txt_PlayerName;

	// Unit Name 세팅
	void SetPlayerName(const FString& unitName);
	
	// Unit의 갑옷에 있는 Armor 수치
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* PGB_PlayerArmor;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* txt_PlayerArmor;

	// Unit의 HP 수치
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* PGB_PlayerHP;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* txt_PlayerHP;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* txt_PlayerMaxHP;
	
	void SetPlayerHPUI(class ABaseBattlePawn* unit);
	void PlayerUpdateHP(class ABaseBattlePawn* unit, int32 hp);
};
