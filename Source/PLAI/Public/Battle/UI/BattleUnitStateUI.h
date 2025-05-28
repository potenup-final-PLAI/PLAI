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
	//----------------Base UI ------------------------
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* WS_BattleUnitState;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* PGB_BaseHP;

	void ShowBaseUI();
	//----------------Hover UI------------------------
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
	
	void ShowHoverUI();
	void SetHPUI(class ABaseBattlePawn* unit);
	void UpdateHP(int32 hp, ABaseBattlePawn* unit);

	//----------------PrintSkillName UI------------------------
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* txt_PrintSkillName;

	void ShowPrintSkillNameUI();
	void SetPrintSkillName(const FString& skillName);

	//----------------API Reason UI--------------------
	UPROPERTY(meta=(BindWidget))
	class UMultiLineEditableTextBox* TB_APIReason;

	void ShowAPIReasonUI();
	void SetAPIReason(const FString& dialogue);

	FString GetNetModeString(UWorld* World);

	//-------------Update HP----------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	int32 curHP = 0;
	void UpdatePlayerHPUI(int32 hp);
	void UpdateEnemyHPUI(int32 hp);
};


