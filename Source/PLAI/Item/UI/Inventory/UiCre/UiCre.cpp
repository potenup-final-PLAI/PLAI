// Fill out your copyright notice in the Description page of Project Settings.


#include "UiCre.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UUiCre::SetUiCre(FCreStruct* CreStruct)
{
	Name->SetText(FText::FromString(CreStruct->Name));
	
	Level->SetText(FText::AsNumber(CreStruct->Level));
	CurrentHp->SetText(FText::AsNumber(CreStruct->CurrentHp));
      	MaxHp->SetText(FText::AsNumber(CreStruct->MaxHp));
	HpBar->SetPercent(float(CreStruct->CurrentHp) / CreStruct->MaxHp);
	
	CurrentExp->SetText(FText::AsNumber(CreStruct->CurrentExp));
    	MaxExp->SetText(FText::AsNumber(CreStruct->MaxExp));
	ExpBar->SetPercent(float(CreStruct->CurrentExp) / CreStruct->MaxExp);
}
