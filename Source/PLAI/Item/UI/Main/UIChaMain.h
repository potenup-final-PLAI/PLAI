// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PLAI/Item/Login/UserStruct.h"
#include "UIChaMain.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUIChaMain : public UUserWidget
{
	GENERATED_BODY()

public:
	// 왼족 위 캐릭터 상세창
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Name;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Job;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Gen;
	// UPROPERTY(meta = (BindWidget))
	// class UTextBlock* Traits;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* TraitsName;
	
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* TraitsStatNameBox;

	// UPROPERTY(meta = (BindWidget))
	// class UVerticalBox* TraitsStatValueBox;

	// 오른쪽 위 캐릭터
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Level;
	
	UPROPERTY(meta = (BindWidget))//아직못함
	class UProgressBar* Exp;
	UPROPERTY(meta = (BindWidget))//아직못함
	class UProgressBar* Hp;
	UPROPERTY(meta = (BindWidget))//아직못함
	class UTextBlock* HpSco;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Local;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Gold;
	
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
	class UTextBlock* Crid;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Spe;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Me;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_SelectMode;
	
	UPROPERTY(EditAnywhere)
	class UUiMain* UiMain;

	void SetUiChaStat(FUserFullInfo* UserFullInfo);

	UFUNCTION()
	void OnLoadMeInfo();

	UFUNCTION()
	void OnSelectMode();
	
	virtual void NativeConstruct() override;
};
