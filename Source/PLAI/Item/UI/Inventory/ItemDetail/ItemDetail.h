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
	class UTextBlock* StatA;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StatB;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StatC;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StatD;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StatE;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StatNameA;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StatNameB;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StatNameC;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StatNameE;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;

	void SetItemDetail(const FItemStructTable& ItemStruct);
};
