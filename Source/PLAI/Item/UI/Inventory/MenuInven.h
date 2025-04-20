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
	class UItemInven* WBP_ItemInven;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UEquipInven* WBP_EquipInven;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UItemDetail* WBP_ItemDetail;
};
