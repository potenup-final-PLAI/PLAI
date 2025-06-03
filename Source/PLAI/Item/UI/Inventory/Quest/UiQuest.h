// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UiQuest.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUiQuest : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere)
	float CurrentTime = 0.0;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuestName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuestContentBlock;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* QuestBoxVertical;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* QuestBox;
	
	void NextQuest(int32 QuestNum, FString QuestTitle, FString QuestContent);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
