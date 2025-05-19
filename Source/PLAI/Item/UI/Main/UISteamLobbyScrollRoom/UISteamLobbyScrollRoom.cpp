// Fill out your copyright notice in the Description page of Project Settings.


#include "UISteamLobbyScrollRoom.h"

#include "Components/Button.h"
#include "PLAI/Item/GameInstance/WorldGi.h"
#include "PLAI/Item/Login/LoginComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Main/UiMain.h"

void UUISteamLobbyScrollRoom::NativeConstruct()
{
	Super::NativeConstruct();

	Button_JoinRoom->OnClicked.AddDynamic(this, &UUISteamLobbyScrollRoom::OnJoinRoom);
}

void UUISteamLobbyScrollRoom::OnJoinRoom()
{
	if (UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
	{
		WorldGi->JoinOtherSession();
			
		WorldGi->UserFullInfoGi = UiMain->LoginComp->TestPlayer->LoginComp->UserFullInfo;
		WorldGi->bGameStart = true;
		WorldGi->bBattleReward = false;
	}
}
