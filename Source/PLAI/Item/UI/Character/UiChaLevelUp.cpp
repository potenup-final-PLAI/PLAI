// Fill out your copyright notice in the Description page of Project Settings.


#include "UiChaLevelUp.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UUiChaLevelUp::SetOpacity(float Opacity)
{
	LevelUp->SetOpacity(Opacity);
	LevelUpCount->SetOpacity(Opacity);
	LevelUpImage->SetOpacity(Opacity);

	if (Opacity > 0.5)
	
	LevelUp->SetRenderScale(FVector2d(Opacity * 1.6,Opacity * 1.6));
	LevelUp->SetRenderScale(FVector2d(Opacity * 1.6,Opacity * 1.6));
	LevelUp->SetRenderScale(FVector2d(Opacity * 1.6,Opacity * 1.6));
}
