// Fill out your copyright notice in the Description page of Project Settings.

#include "UiSteamLobby.h"

#include "UiMain.h"
#include "Components/Button.h"
#include "PLAI/Item/GameInstance/WorldGi.h"
#include "PLAI/Item/Login/LoginComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"

void UUiSteamLobby::NativeConstruct()
{
	Super::NativeConstruct();

	Button_CreateRoom->OnClicked.AddDynamic(this,&UUiSteamLobby::OnCreateRoom);
	Button_FindRoom->OnClicked.AddDynamic(this,&UUiSteamLobby::OnFindRoom);
	Button_JoinRoom->OnClicked.AddDynamic(this,&UUiSteamLobby::OnJoinRoom);
}


void UUiSteamLobby::OnCreateRoom()
{
	if (UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
	{
		WorldGi->CreateSession(FString("PLAI Game"),4);
	}
}

void UUiSteamLobby::OnFindRoom()
{
	if (UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
	{
		WorldGi->FindOtherSession();
		
		WorldGi->UserFullInfoGi = UiMain->LoginComp->TestPlayer->LoginComp->UserFullInfo;
		WorldGi->bGameStart = true;
		WorldGi->bBattleReward = false;
		
	}
}

void UUiSteamLobby::OnJoinRoom()
{
	if (UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
	{
		WorldGi->JoinOtherSession();
		
		// UiMain->LoginComp->TestPlayer->LogItemComp->GetInvenInfo();
		// UiMain->LoginComp->TestPlayer->LogItemComp->GetEquipInfo();
		// UiMain->LoginComp->TestPlayer->LogItemComp->GetUserLevel();
			
		WorldGi->UserFullInfoGi = UiMain->LoginComp->TestPlayer->LoginComp->UserFullInfo;
		WorldGi->bGameStart = true;
		WorldGi->bBattleReward = false;
	}
}
