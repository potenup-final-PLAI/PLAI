// Fill out your copyright notice in the Description page of Project Settings.


#include "UiSUbMain.h"

#include "UiChaView.h"
#include "UiMain.h"
#include "UiSteamLobby.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "PLAI/Item/GameInstance/WorldGi.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/Login/LoginComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Inventory/ItemDetail/ItemDetail.h"
#include "PLAI/Item/UI/Inventory/QuickInven/QuickInven.h"
#include "PLAI/Item/UI/Inventory/UiCre/UiCre.h"
#include "PLAI/Item/UI/Slot/SlotCre.h"

void UUiSUbMain::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonSingle->OnClicked.AddDynamic(this,&UUiSUbMain::OnButtonSingle);
	ButtonMulti->OnClicked.AddDynamic(this,&UUiSUbMain::OnButtonMulti);
}

void UUiSUbMain::OnButtonSingle()
{
	UGameplayStatics::SetGamePaused(GetWorld(),false);
	
	UiMain->LoginComp->TestPlayer->InvenComp->MenuInven->WBP_QuickInven->SetVisibility(ESlateVisibility::Visible);
	UiMain->LoginComp->TestPlayer->InvenComp->MenuInven->WBP_SlotCre->SetVisibility(ESlateVisibility::Visible);
	
	UiMain->LoginComp->TestPlayer->InvenComp->MenuInven->Wbp_UiCre->SetVisibility(ESlateVisibility::Visible);
	UiMain->LoginComp->TestPlayer->InvenComp->MenuInven->Wbp_ChaView->SetVisibility(ESlateVisibility::Visible);
	UiMain->LoginComp->TestPlayer->InvenComp->MenuInven->Wbp_ItemGold->SetVisibility(ESlateVisibility::Visible);
	
	RemoveFromParent();
	UiMain->Wbp_UiSteamLobby->RemoveFromParent();

	if (UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
	{ WorldGi->bLoginMe = true; UE_LOG(LogTemp,Warning,TEXT("UiSubMain bLoginMe 바꼈냐 [%d]"),WorldGi->bLoginMe); }
}

void UUiSUbMain::OnButtonMulti()
{
	if (UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
	{
		RemoveFromParent();
	}
}
