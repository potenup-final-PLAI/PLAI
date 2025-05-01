// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UiChaView.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUiChaView : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	class UTextBlock* NameCha;
};
