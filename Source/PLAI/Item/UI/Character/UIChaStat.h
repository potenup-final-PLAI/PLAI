// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIChaStat.generated.h"

struct FUserFullInfo;
/**
 * 
 */
UCLASS()
class PLAI_API UUIChaStat : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Name;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Job;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Gen;
	
	// UPROPERTY(meta = (BindWidget))
	// class UVerticalBox* TraitsName;
	//
	// UPROPERTY(meta = (BindWidget))
	// class UVerticalBox* TraitsStatNameBox;
	
	// 오른쪽 위 캐릭터
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Level;
	
	UPROPERTY(meta = (BindWidget))//아직못함
	class UProgressBar* ExpBar;
	UPROPERTY(meta = (BindWidget))//아직못함
	class UProgressBar* HpBar;
	// UPROPERTY(meta = (BindWidget))//아직못함
	// class UTextBlock* HpSco;
	
	// 오리지날 스텟
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HP;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Atk;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Def;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Res;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Cri;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CriD;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Spd;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Mov;

	// 특성 스텟
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HpT;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AtkT;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DefT;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ResT;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CriT;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CriDT;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SpdT;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MovT;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HpE;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AtkE;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DefE;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ResE;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CriE;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CriDE;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SpdE;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MovE;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HpTot;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AtkTot;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DefTot;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ResTot;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CriTot;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CriDTot;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SpdTot;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MovTot;

	UPROPERTY(meta = (BindWidget))
	class UGridPanel* TraitsBox;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentExp;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MaxExp;

	UPROPERTY(EditAnywhere)
    UDataTable* TraitStructTable;
	
	void SetUiChaStat(FUserFullInfo* UserFullInfo);
};
