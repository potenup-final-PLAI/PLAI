// Fill out your copyright notice in the Description page of Project Settings.

#include "UiSteamLobby.h"

#include "UiMain.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "PLAI/Item/GameInstance/WorldGi.h"
#include "PLAI/Item/Login/LoginComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"

void UUiSteamLobby::NativeConstruct()
{
	Super::NativeConstruct();

	Button_CreateRoom->OnClicked.AddDynamic(this,&UUiSteamLobby::OnCreateRoom);
	Button_FindRoom->OnClicked.AddDynamic(this,&UUiSteamLobby::OnFindRoom);
	// Button_JoinRoom->OnClicked.AddDynamic(this,&UUiSteamLobby::OnJoinRoom);

	if (UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
	{
		WorldGi->OnFindSession.AddUObject(this, &UUiSteamLobby::AddScrollBox);
	}
}


void UUiSteamLobby::AddScrollBox(FString SessionName)
{
	SteamLobbyScrollRoom = CreateWidget<UUISteamLobbyScrollRoom>(GetWorld(),ScrollRoomFactory);
	SteamLobbyScrollRoom->UiMain = UiMain;
	SteamLobbyScrollRoom->JoinName->SetText(FText::FromString(SessionName));
	ScrollBox->AddChild(SteamLobbyScrollRoom);
}

void UUiSteamLobby::OnCreateRoom()
{
	if (UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
	{
		// UiMain->LoginComp->TestPlayer->LogItemComp->GetInvenInfo();
		// UiMain->LoginComp->TestPlayer->LogItemComp->GetEquipInfo();
		UiMain->LoginComp->TestPlayer->LogItemComp->GetUserLevel();

		FString TitleName = RoomName->GetText().ToString();
		
		WorldGi->CreateSession(TitleName,4);
		WorldGi->UserFullInfoGi = UiMain->LoginComp->TestPlayer->LoginComp->UserFullInfo;
		
		WorldGi->bLoginMe = true;
		WorldGi->bGameStart = true;
		WorldGi->bBattleReward = false;
	}
}

void UUiSteamLobby::OnFindRoom()
{
	if (UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
	{
		WorldGi->FindOtherSession();
	}
}

// void UUiSteamLobby::OnJoinRoom()
// {
// 	if (UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
// 	{
// 		// UiMain->LoginComp->TestPlayer->LogItemComp->GetInvenInfo();
// 		// UiMain->LoginComp->TestPlayer->LogItemComp->GetEquipInfo();
// 		UiMain->LoginComp->TestPlayer->LogItemComp->GetUserLevel();
// 		
// 		WorldGi->JoinOtherSession();
// 			
// 		WorldGi->UserFullInfoGi = UiMain->LoginComp->TestPlayer->LoginComp->UserFullInfo;
//
// 		WorldGi->bLoginMe = true;
// 		WorldGi->bGameStart = true;
// 		WorldGi->bBattleReward = false;
// 	}
// }
