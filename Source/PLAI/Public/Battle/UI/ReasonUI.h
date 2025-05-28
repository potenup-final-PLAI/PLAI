// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReasonUI.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UReasonUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_Reason;

	void SetText(FString text);

};
