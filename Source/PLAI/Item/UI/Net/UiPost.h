// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

DECLARE_DELEGATE_OneParam(FOnNetPost,FString String)

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UiPost.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUiPost : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextGet;
	
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* TextPost;

	UPROPERTY(EditAnywhere)
	class ANpcNet* NpcNet;

	FOnNetPost OnNetPost;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void UiPostEnter(const FText& Text, ETextCommit::Type CommitMethod);
};
