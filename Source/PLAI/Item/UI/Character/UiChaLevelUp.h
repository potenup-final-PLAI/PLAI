// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UiChaLevelUp.generated.h"

class UMediaPlayer;
class UMediaSource;
class UMediaTexture;

/**
 * 
 */
UCLASS()
class PLAI_API UUiChaLevelUp : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LevelUpCount;
	
	UPROPERTY(meta = (BindWidget))
    class UTextBlock* LevelUp;

	UPROPERTY(meta = (BindWidget))
	class UImage* LevelUpImage;

	void SetOpacity(float Opacity);
	// UPROPERTY(EditAnywhere)
	// class UMediaPlayer*  MediaPlayer;
	//
	// UPROPERTY(EditAnywhere)
	// class UMediaSource*  MediaSource;
	//
	// UPROPERTY(EditAnywhere)
	// class UMediaTexture* MediaTexture;
};
