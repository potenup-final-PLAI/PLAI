// Fill out your copyright notice in the Description page of Project Settings.


#include "UiMain.h"

#include "JsonObjectConverter.h"
#include "UiSign.h"
#include "Components/Button.h"

void UUiMain::SetUiSign()
{
	FString JsonString;
	FString Path = FString::Printf(TEXT("%s%s"),*FPaths::ProjectDir(),TEXT("/SaveSign/SignSave.txt"));
	FFileHelper::LoadFileToString(JsonString,*Path);

	FSignStructs SignStructs;
	
	FJsonObjectConverter::JsonObjectStringToUStruct(JsonString,&SignStructs);
	UE_LOG(LogTemp,Warning,TEXT("UiMain 가입정보 불러오기 %s"),*JsonString);
	
	if (bUiSign == false)
	{ WbpUiSign->SetVisibility(ESlateVisibility::Visible);
		bUiSign = true; }
	else
	{ WbpUiSign->SetVisibility(ESlateVisibility::Hidden);
		bUiSign = false; }
}

void UUiMain::Login()
{
	
}

void UUiMain::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonStart->OnClicked.AddDynamic(this,&UUiMain::RemoveFromParent);
	ButtonSignin->OnClicked.AddDynamic(this,&UUiMain::SetUiSign);
	
	WbpUiSign->SetVisibility(ESlateVisibility::Hidden);
}
