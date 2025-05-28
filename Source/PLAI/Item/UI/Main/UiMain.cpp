// Fill out your copyright notice in the Description page of Project Settings.


#include "UiMain.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "UIChaMain.h"
#include "UIinitMain.h"
#include "UiSign.h"
#include "UiSteamLobby.h"
#include "UiSUbMain.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/EditableTextBox.h"
#include "Components/SizeBox.h"
#include "Kismet/GameplayStatics.h"
#include "PLAI/Item/Login/LoginComp.h"
#include "Components/TextBlock.h"
#include "Interfaces/IHttpResponse.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Character/UiChaLevelUp.h"
#include "PLAI/Item/UI/Character/UIChaStat.h"
#include "PLAI/Item/UI/Inventory/EquipInven/EquipInven.h"
#include "PLAI/Item/UI/Inventory/InputUi/InputUi.h"
#include "PLAI/Item/UI/Inventory/ItemDetail/ItemDetail.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemInven.h"

void UUiMain::NativeConstruct()
{
	Super::NativeConstruct();

	// UGameplayStatics::SetGamePaused(GetWorld(),true);

    Wbp_UIChaMain->UiMain = this;
	Wbp_UiSubMain->UiMain = this;
	Wbp_UiInitMain->UiMain = this;
	Wbp_UiSteamLobby->UiMain = this;
	
	if (APlayerController* pc = Cast<APlayerController>(GetOwningPlayer()))
		
	ButtonStart->OnClicked.AddDynamic(this,&UUiMain::OnButtonStart);
	ButtonSignin->OnClicked.AddDynamic(this,&UUiMain::SetUiSign);
	ButtonLogin->OnClicked.AddDynamic(this,&UUiMain::Login);
	
	ButtonInitPost->OnClicked.AddDynamic(this,&UUiMain::HttpPostInit);
	ButtonInitEnd->OnClicked.AddDynamic(this,&UUiMain::InitEnd);

	ButtonCreate->OnClicked.AddDynamic(this,&UUiMain::CreateCharacter);

	loginFailed->SetVisibility(ESlateVisibility::Hidden);;
	WbpUiSign->SetVisibility(ESlateVisibility::Hidden);
}

void UUiMain::SetUiSign()
{
	if (bUiSign == false)
	{ WbpUiSign->SetVisibility(ESlateVisibility::Visible);
		WbpUiSign->SignCompleteBox->SetVisibility(ESlateVisibility::Hidden);
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
		{
			CanvasMain->RemoveFromParent();
			//여기까지 아이디는 들어왔음
		}
		else
		{   loginFailed->SetVisibility(ESlateVisibility::Visible);
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
			{ loginFailed->SetVisibility(ESlateVisibility::Hidden);},1.5f,false);
		}
	});
	LoginComp->HttpLoginPost();
}

void UUiMain::CreateCharacter()
{
	FString CharacterName = CreatePost->GetText().ToString();
	LoginComp->HttpCreatePost(CharacterName);
	CanvasInit->RemoveFromParent();
}

void UUiMain::HttpLoginMe()
{
	FHttpRequestRef httpRequest = FHttpModule::Get().CreateRequest(); FNgrok Ngrok;
	FString url = FString::Printf(TEXT("%s/me/"),*Ngrok.Ngrok);

	httpRequest->SetURL(url);
	httpRequest->SetVerb("POST");
	httpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	FString JsonString;
	FMeStruct MeStruct;
	
	MeStruct.user_id = LoginComp->UserFullInfo.user_id;
	
	FJsonObjectConverter::UStructToJsonObjectString(MeStruct,JsonString);
	httpRequest->SetContentAsString(JsonString);
	httpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bProcessedSuccessfully)
	{
		if (bProcessedSuccessfully)
		{
			FString JsonString = HttpResponse->GetContentAsString();
			UE_LOG(LogTemp,Warning,TEXT("로그인컴프 나의정보 GetAsContetAsString된 제이슨값 성공 %s 요청코드는%d"),
				*JsonString,HttpResponse->GetResponseCode());
			// UE_LOG(LogTemp,Warning,TEXT("로그인컴프 나의정보 조회 성공 %s"),*JsonString);
			FUserFullInfo InitUserFullInfo;
			FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &LoginComp->UserFullInfo);
			UE_LOG(LogTemp,Warning,TEXT("로그인컴프 나의 캐릭터 아이디 [%s]"),*LoginComp->UserFullInfo.character_info.character_name);

			LoginComp->ConnectWebSocket();

			if (LoginComp->UserFullInfo.character_info.character_name != InitUserFullInfo.character_info.character_name)
			{
				Wbp_UiInitMain->RemoveFromParent();

				UGameplayStatics::SetGamePaused(GetWorld(),false);
				LoginComp->TestPlayer->InvenComp->MenuInven->AddToViewport(1);
				LoginComp->TestPlayer->InvenComp->MenuInven->WBP_EquipInven->SetVisibility(ESlateVisibility::Hidden);
				LoginComp->TestPlayer->InvenComp->MenuInven->WBP_ItemInven->SetVisibility(ESlateVisibility::Hidden);
				LoginComp->TestPlayer->InvenComp->MenuInven->WBP_ItemDetail->SetVisibility(ESlateVisibility::Hidden);
				LoginComp->TestPlayer->InvenComp->MenuInven->Wbp_UIChaStat->SetVisibility(ESlateVisibility::Hidden);
				LoginComp->TestPlayer->InvenComp->MenuInven->WBP_InputUi->SetVisibility(ESlateVisibility::Hidden);
				LoginComp->TestPlayer->InvenComp->MenuInven->Wbp_UiChaLevelUp->SetVisibility(ESlateVisibility::Hidden);
				LoginComp->HttpMePost();
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this](){ UGameplayStatics::SetGamePaused
					(GetWorld(),true); },1.0f,false);
					
			}
			// FString GetJson;
			// FJsonObjectConverter::UStructToJsonObjectString(&LoginComp->UserFullInfo,GetJson);
			// UE_LOG(LogTemp,Warning,TEXT("로그인컴프 나의정보 조회 Json변환 %s"),*GetJson);
			// Wbp_UIChaMain->SetUiChaStat(&LoginComp->UserFullInfo);
		}
	});
	httpRequest->ProcessRequest();
}

void UUiMain::OnButtonStart()
{
	UGameplayStatics::SetGamePaused(GetWorld(),false);
	
	LoginComp->TestPlayer->InvenComp->MenuInven->AddToViewport(1);
	LoginComp->TestPlayer->InvenComp->MenuInven->WBP_ItemDetail->SetVisibility(ESlateVisibility::Hidden);
	LoginComp->TestPlayer->InvenComp->MenuInven->WBP_EquipInven->SetVisibility(ESlateVisibility::Hidden);
	LoginComp->TestPlayer->InvenComp->MenuInven->WBP_ItemInven->SetVisibility(ESlateVisibility::Hidden);
	LoginComp->TestPlayer->InvenComp->MenuInven->Wbp_UIChaStat->SetVisibility(ESlateVisibility::Hidden);
	LoginComp->TestPlayer->InvenComp->MenuInven->WBP_InputUi->SetVisibility(ESlateVisibility::Hidden);
	LoginComp->TestPlayer->InvenComp->MenuInven->Wbp_UiChaLevelUp->SetVisibility(ESlateVisibility::Hidden);
	
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