// Fill out your copyright notice in the Description page of Project Settings.


#include "UiSign.h"

#include "JsonObjectConverter.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"

void UUiSign::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonSign->OnClicked.AddDynamic(this,&UUiSign::OnSignin);
}

void UUiSign::OnSignin()
{
	FString id = SignId->GetText().ToString();
    FString pw = SignPw->GetText().ToString();
	UE_LOG(LogTemp,Display,TEXT("Sign in Id%s Pw%s"),*id,*pw);

	SignStruct.Id = id;
	SignStruct.Pw = pw;

	FString JsonString;
	SignStructs.SignStructs.Add(SignStruct);
	FJsonObjectConverter::UStructToJsonObjectString(SignStructs,JsonString);

	FString path = FString::Printf(TEXT("%s/%s"),*FPaths::ProjectDir(),TEXT("SaveSign/SignSave.txt"));
	FFileHelper::SaveStringToFile(JsonString,*path);
}
