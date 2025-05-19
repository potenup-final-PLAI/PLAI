// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UiSUbMain.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUiSUbMain : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget = "Widget1"))
	class UButton* ButtonSingle;

	UPROPERTY(meta = (BindWidget = "Widget1"))
	class UButton* ButtonMulti;

	UPROPERTY(EditAnywhere)
	class UUiMain* UiMain;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnButtonSingle();

	UFUNCTION()
	void OnButtonMulti();
};
