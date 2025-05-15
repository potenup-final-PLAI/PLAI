// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UiNpcStart.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUiNpcStart : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StartJobText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextA;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextB;
};
