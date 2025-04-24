// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UiMain.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUiMain : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonStart;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonLogin;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonSignin;

	UPROPERTY(meta = (BindWidget))
	class UUiSign* WbpUiSign;

	bool bUiSign = false;
	
	UFUNCTION()
	void SetUiSign();
	void Login();

	virtual void NativeConstruct() override;
};
