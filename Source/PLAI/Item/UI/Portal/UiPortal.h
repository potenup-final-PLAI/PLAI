// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UiPortal.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUiPortal : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Village;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Dessert;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Mountain;
	
	UPROPERTY(EditAnywhere)
	class AWarp* Warp;
	
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnButton_Village();

	UFUNCTION()
	void OnButton_Mountain();
	
	UFUNCTION()
	void OnButton_Dessert();
};
