// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIWorldMapGuide.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUIWorldMapGuide : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere)
	float CurrentTime = 0.0f;

	virtual void NativeDestruct() override;
};
