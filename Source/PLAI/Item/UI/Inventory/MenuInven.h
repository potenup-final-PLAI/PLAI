// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuInven.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UMenuInven : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UInputUi* WBP_InputUi;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UItemInven* WBP_ItemInven;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UEquipInven* WBP_EquipInven;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UItemDetail* WBP_ItemDetail;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UQuickInven* WBP_QuickInven;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UUIChaStat* Wbp_UIChaStat;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class USlotCre* WBP_SlotCre;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UUiChaView* Wbp_ChaView;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UUiCre* Wbp_UiCre;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UItemGold* Wbp_ItemGold;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UUiChaLevelUp* Wbp_UiChaLevelUp;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UUiWorldMap* Wbp_UiWorldMap;

	virtual void NativeConstruct() override;
};
