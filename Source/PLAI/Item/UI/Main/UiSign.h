// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UiSign.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUiSign : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* SignId;
	
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* SignPw;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonSign;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnSignin();
};
