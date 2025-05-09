// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIChaStat.generated.h"

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
	
	// 오른쪽 캐릭터 스텟창
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
	class UTextBlock* Spe;
	
	void SetUiChaStat(FUserFullInfo* UserFullInfo);
};
