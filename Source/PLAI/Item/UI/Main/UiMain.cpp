// Fill out your copyright notice in the Description page of Project Settings.


#include "UiMain.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "UiSign.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"
#include "PLAI/Item/Login/LoginComp.h"
#include "Components/TextBlock.h"
#include "Interfaces/IHttpResponse.h"

void UUiMain::NativeConstruct()
{
	Super::NativeConstruct();

	UGameplayStatics::SetGamePaused(GetWorld(),true);
	
	if (APlayerController* pc = Cast<APlayerController>(GetOwningPlayer()))
		
	ButtonStart->OnClicked.AddDynamic(this,&UUiMain::OnButtonStart);
	ButtonSignin->OnClicked.AddDynamic(this,&UUiMain::SetUiSign);
	ButtonLogin->OnClicked.AddDynamic(this,&UUiMain::Login);
	
	ButtonInitPost->OnClicked.AddDynamic(this,&UUiMain::HttpPostInit);
	ButtonInitEnd->OnClicked.AddDynamic(this,&UUiMain::InitEnd);

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
		{   CanvasMain->RemoveFromParent(); }
		else
		{   LoginFail->SetVisibility(ESlateVisibility::Visible);
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
			{ LoginFail->SetVisibility(ESlateVisibility::Hidden);},1.5f,false);
		}
	});
	LoginComp->HttpLoginPost();

    // 밑에거는 클라전용 회원가입 란
	
	// FString JsonString;
	// FString Path = FString::Printf(TEXT("%s%s"),*FPaths::ProjectDir(),TEXT("/SaveSign/SignSave.txt"));
	// FFileHelper::LoadFileToString(JsonString,*Path);
	//
	// FSignStructs SignStructs;
	// FJsonObjectConverter::JsonObjectStringToUStruct(JsonString,&SignStructs);
	// bool bLogin = true;
	
   //  for (const FSignStruct& SignStruct : SignStructs.SignStructs)
   //  {
   //  	if (SignStruct.Id == LoginId->GetText().ToString()
   //  		&& SignStruct.Pw == LoginPw->GetText().ToString())
   //  	{
   //  		LoginComp->UserId = LoginId->GetText().ToString();
   //  		LoginComp->UserId = LoginPw->GetText().ToString();
   //  		UE_LOG(LogTemp,Display,TEXT("UiSign 사인 Successful"));
   //  		CanvasMain->RemoveFromParent();
   //  	}
   //  	else
   //  	{
   //  		LoginFail->SetVisibility(ESlateVisibility::Visible);
   //  		FTimerHandle TimerHandle;
   //  		GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
			// { LoginFail->SetVisibility(ESlateVisibility::Hidden);},1.5f,false);
   //  		UE_LOG(LogTemp,Display,TEXT("UiSign 사인 실패"));
   //  	}
    // }
}

void UUiMain::OnButtonStart()
{
	UGameplayStatics::SetGamePaused(GetWorld(),false);
	RemoveFromParent();
}

void UUiMain::InitEnd()
{
	UE_LOG(LogTemp,Warning,TEXT("UiSubMainInitButtontrue"));
	CanvasInit->RemoveFromParent();
}

void UUiMain::HttpPostInit()
{
	FHttpRequestRef httpRequest = FHttpModule::Get().CreateRequest();

    UE_LOG(LogTemp,Display,TEXT("HttpPostInit 캐릭터 초기 셋팅"));
	
	httpRequest->SetURL("http://192.168.10.96:8054/npc/chat");
	httpRequest->SetVerb("POST");
	httpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	FString JsonString;
	FNpcStructPost NpcStruct;
	NpcStruct.question = InitPost->GetText().ToString();
	
	FJsonObjectConverter::UStructToJsonObjectString(NpcStruct,JsonString);
	httpRequest->SetContentAsString(JsonString);
	httpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
	{
		if (WasSuccessful)
		{
			FNpcStructGet NpcStructGet;
			FString JsonString = Response->GetContentAsString();
			FJsonObjectConverter::JsonObjectStringToUStruct(JsonString,&NpcStructGet);
			InitResponse->SetText(FText::FromString(NpcStructGet.response));
		}
	});
	httpRequest->ProcessRequest();
}


