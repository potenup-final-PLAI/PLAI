// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemStruct.h"
#include "UObject/Object.h"
#include "ItemObject.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UItemObject : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere)
	FItemStruct ItemStruct;

	UPROPERTY(EditAnywhere)
	FItemStructTable ItemStructTable;

	UPROPERTY(EditAnywhere)
	class USlot* SlotUi;

	// UPROPERTY(EditAnywhere)
	// class USlot* SlotUiPre;

	UPROPERTY(EditAnywhere)
	class ATestPlayer* TestPlayer;

	UPROPERTY(EditAnywhere)
	bool bBuy = false;
};
