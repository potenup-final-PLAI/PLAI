// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UiChaView.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUiChaView : public UUserWidget
{
	GENERATED_BODY()
// 이것은 왼쪽 위에있는 캐릭터 셋팅창임
public:
	UPROPERTY(EditAnywhere)
	class UTextBlock* NameCha;
};
