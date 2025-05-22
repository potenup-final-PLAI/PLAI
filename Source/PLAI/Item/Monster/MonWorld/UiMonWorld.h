// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UiMonWorld.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUiMonWorld : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Yes;
    UFUNCTION()
	void OnButtonYes();
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_No;
	UFUNCTION()
	void OnButtonNo();

	UPROPERTY()
	class ATestPlayer* TestPlayer;
	
	UPROPERTY()
	class AMonWorld* MonWorld;

	virtual void NativeConstruct() override;

	void ChangeNpcPersonality();
};
