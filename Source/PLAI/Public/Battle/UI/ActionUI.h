// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActionUI.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UActionUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	//-----------Turn End Button------------------
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_TurnEnd;
	UFUNCTION()
	void OnClickedTurnEnd();
	//-----------Move Button------------------
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_Move;

	UFUNCTION()
	void OnClickedMove();
	//-----------FirstSkill Button------------------
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_FirstSkill;

	UFUNCTION()
	void OnClickedFirstSkill();
	//-----------SecondSkill Button------------------
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_SecondSkill;

	UFUNCTION()
	void OnClickedSecondSkill();
};
