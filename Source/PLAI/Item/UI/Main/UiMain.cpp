// Fill out your copyright notice in the Description page of Project Settings.


#include "UiMain.h"

#include "JsonObjectConverter.h"
#include "UiSign.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "PLAI/Item/Login/LoginComp.h"

void UUiMain::NativeConstruct()
{
	Super::NativeConstruct();
	if (APlayerController* pc = Cast<APlayerController>(GetOwningPlayer()))
	{
		UE_LOG(LogTemp,Warning,TEXT("UiMain 엑터 연결")) }
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("UiMain 엑터 연결안됨")) }

	ButtonStart->OnClicked.AddDynamic(this,&UUiMain::RemoveFromParent);
	ButtonSignin->OnClicked.AddDynamic(this,&UUiMain::SetUiSign);
	ButtonLogin->OnClicked.AddDynamic(this,&UUiMain::Login);

	LoginFail->SetVisibility(ESlateVisibility::Hidden);;
	WbpUiSign->SetVisibility(ESlateVisibility::Hidden);
}

void UUiMain::SetUiSign()
{
	if (bUiSign == false)
	{ WbpUiSign->SetVisibility(ESlateVisibility::Visible);
		bUiSign = true; }
	else
	{ WbpUiSign->SetVisibility(ESlateVisibility::Hidden);
		bUiSign = false; }
}

void UUiMain::Login()
{
	LoginComp->OnLogin.BindLambda([this](bool bLogin)
	{
		if (bLogin)
		{   RemoveFromParent(); }
		else
		{   LoginFail->SetVisibility(ESlateVisibility::Visible);
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
			{ LoginFail->SetVisibility(ESlateVisibility::Hidden);},1.5f,false);
		}
	});
	LoginComp->HttpLoginPost();

    // 밑에거는 클라전용 회원가입 란
	
	FString JsonString;
	FString Path = FString::Printf(TEXT("%s%s"),*FPaths::ProjectDir(),TEXT("/SaveSign/SignSave.txt"));
	FFileHelper::LoadFileToString(JsonString,*Path);
	
	FSignStructs SignStructs;
	FJsonObjectConverter::JsonObjectStringToUStruct(JsonString,&SignStructs);
	bool bLogin = true;
	
    for (const FSignStruct& SignStruct : SignStructs.SignStructs)
    {
    	if (SignStruct.Id == LoginId->GetText().ToString()
    		&& SignStruct.Pw == LoginPw->GetText().ToString())
    	{
    		LoginComp->UserId = LoginId->GetText().ToString();
    		LoginComp->UserId = LoginPw->GetText().ToString();
    		UE_LOG(LogTemp,Display,TEXT("UiSign 사인 Successful"));
    		bLogin = true;
    	}
    	else
    	{
    		bLogin = false;
    		UE_LOG(LogTemp,Display,TEXT("UiSign 사인 실패"));
    	}
    	
    	if (bLogin == false)
    	{
    		LoginFail->SetVisibility(ESlateVisibility::Visible);
    		FTimerHandle TimerHandle;
    		GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
    		{ LoginFail->SetVisibility(ESlateVisibility::Hidden);},1.5f,false);
    	}
    	else
    	{
    		RemoveFromParent();
    	}
    }
}


