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
	//-----------객체 저장-------------------------
	UPROPERTY(EditAnywhere)
	class AUPhaseManager* phaseManager;
	UPROPERTY(EditAnywhere)
	class ATurnManager* turnManager;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ATurnManager> turnManagerFactory;


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
	//-----------BaseAttack Button------------------
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_BaseAttack;
	
	UFUNCTION()
	void OnClickedBaseAttack();
	//-----------SecondSkill Button------------------
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_Poison;

	UFUNCTION()
	void OnClickedPoison();
	//-----------SecondSkill Button------------------
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_Fatal;

	UFUNCTION()
	void OnClickedFatal();
	//-----------SecondSkill Button------------------
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_Rupture;
	
	UFUNCTION()
	void OnClickedRupture();

	class ABattlePlayer* IsMyTurn();
};
