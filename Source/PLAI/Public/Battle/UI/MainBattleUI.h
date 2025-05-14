// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CycleAndTurn.h"
#include "Blueprint/UserWidget.h"
#include "MainBattleUI.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UMainBattleUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
public:
	UPROPERTY(meta = (BindWidget))
	class UCycleAndTurn* WBP_CycleAndTurn;

	UPROPERTY(meta = (BindWidget))
	class UBattlePlayerInfoUI* WBP_Player;
	
	// AP 추가될 패널
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HB_AP;
	// AP 위젯 클레스
	UPROPERTY(EditAnywhere)
	class TSubclassOf<UUserWidget> apFactory;

	void AddAP();
};
