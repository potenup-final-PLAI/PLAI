// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIinitMain.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUIinitMain : public UUserWidget
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
	class UUiMain* UiMain;
	
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* CanvasInit;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonInitEnd;
	
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* InitPost;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InitResponse;

	// UPROPERTY(meta = (BindWidget))
	// class UButton* ButtonCreate;
	
	// UPROPERTY(meta = (BindWidget))
	// class UEditableTextBox* CreatePost;
public:
    UFUNCTION()
	void OnInitPostEnter(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnInitEnd();

	// UFUNCTION()
	// void OnCreateCharacter();

    void LoadInitSocekt();
	
	virtual void NativeConstruct() override;
	
};
