// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemGold.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UItemGold : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	class UTextBlock* Gold;

	virtual void NativeConstruct() override;
};
