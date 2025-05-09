// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleUnitStateUI.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UBattleUnitStateUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
public:
	// Unit 이름 
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* txt_UnitName;

	// Unit Name 세팅
	void SetUnitName(const FString& unitName);
	
	// Unit의 갑옷에 있는 Armor 수치
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* PGB_Armor;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* txt_Armor;

	// Unit의 HP 수치
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* PGB_HP;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* txt_HP;
	int32 maxHP = 0;

	
	void SetHPUI(class ABaseBattlePawn* unit);
	void UpdateHP(int32 hp);
	
};


