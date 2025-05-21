// Fill out your copyright notice in the Description page of Project Settings.


#include "UIinitMain.h"

#include "UiMain.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "PLAI/Item/Login/LoginComp.h"


void UUIinitMain::NativeConstruct()
{
	Super::NativeConstruct();

	// 임시 코드임 추후에 버튼으로 소켓 연결 이벤트 추가
	// UiMain->LoginComp->ConnectWebSocket();
	
	InitPost->OnTextCommitted.AddDynamic(this,&UUIinitMain::OnInitPostEnter);
	ButtonInitEnd->OnClicked.AddDynamic(this,&UUIinitMain::OnInitEnd);
	// ButtonCreate->OnClicked.AddDynamic(this,&UUIinitMain::OnCreateCharacter);
}

void UUIinitMain::OnInitPostEnter(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::Type::OnEnter)
	{
		// UE_LOG(LogTemp,Display,TEXT("UiInitMain 엔터키 OnInitPostOnEnter 어떤글자? %s"),*Text.ToString());
		// 추후에 질문 다 끝날떄까지만 if문 추가
		if (UiMain->LoginComp->WebSocket.IsValid() && UiMain->LoginComp->WebSocket->IsConnected())
		{
			UiMain->LoginComp->WebSocket->Send(*Text.ToString());
		}
		else { UE_LOG(LogTemp, Warning, TEXT("WebSocket not connected!")); }
	}
}

void UUIinitMain::OnInitEnd()
{
	RemoveFromParent();
}

void UUIinitMain::LoadInitSocekt()
{
	UiMain->LoginComp->ConnectWebSocket();
}








// void UUIinitMain::OnCreateCharacter()
// {
// 	FString CharacterName = UiMain->LoginComp->UiMain->CreatePost->GetText().ToString();
// 	LoginComp->HttpCreatePost(CharacterName);
// 	CanvasInit->RemoveFromParent();
// }
