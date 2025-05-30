// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "UiSign.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType,Blueprintable)
struct FSignStruct
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	FString Id = FString("id");
	UPROPERTY(EditAnywhere)
	FString Pw = FString("pw");
	UPROPERTY(EditAnywhere)
	FItemStructsArray ItemStructsEquip;
	UPROPERTY(EditAnywhere)
	FItemStructsArray ItemStructsInven;
};

USTRUCT(BlueprintType,Blueprintable)
struct FSignStructs : public FTableRowBase
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere)
	TArray<FSignStruct> SignStructs;
};

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

	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonExit;
	
	UPROPERTY(meta = (BindWidget))
	class USizeBox* SignCompleteBox;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* bSingComplete;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* bSingCompleteDetail;

	UPROPERTY(EditAnywhere)
	class ULoginComp* LoginComp;

	// UPROPERTY(EditAnywhere)
	// FSignStructs SignStructs;

	UPROPERTY(EditAnywhere)
	FSignStruct SignStruct;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnSignin();

	UFUNCTION()
	void OnExit();
};
