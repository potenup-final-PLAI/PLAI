// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UiTurn.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUiTurn : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_TurnStart;

	UPROPERTY(EditAnywhere)
	class AGameStateOpen* Gs;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnTurnStart();
};
