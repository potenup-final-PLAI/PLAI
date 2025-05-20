// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UiWorldPlayerIcon.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUiWorldPlayerIcon : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* Rot;

	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;
};
