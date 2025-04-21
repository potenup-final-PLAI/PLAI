// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PLAI/Item/Item/ItemStruct.h"
#include "ItemDetail.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UItemDetail : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Name;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NameType;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NameDetail;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Description;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage; 

	void SetItemDetail(const FItemStruct& ItemStruct);
};
