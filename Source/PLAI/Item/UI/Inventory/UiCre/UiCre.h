// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PLAI/Item/Creture/Creature.h"
#include "PLAI/Item/Item/ItemStruct.h"
#include "UiCre.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUiCre : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentHp;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MaxHp;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentExp;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MaxExp;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HpBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ExpBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Name;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Level;

	void SetUiCre(FCreStruct* CreStruct);
};
