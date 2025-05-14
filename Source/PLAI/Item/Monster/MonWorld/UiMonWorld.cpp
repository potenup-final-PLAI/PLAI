// Fill out your copyright notice in the Description page of Project Settings.


#include "UiMonWorld.h"

#include "MonWorld.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "PLAI/Item/GameInstance/WorldGi.h"
#include "PLAI/Item/Login/LoginComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"

void UUiMonWorld::NativeConstruct()
{
	Super::NativeConstruct();
	Button_No->OnClicked.AddDynamic(this,&UUiMonWorld::OnButtonNo);
	Button_Yes->OnClicked.AddDynamic(this,&UUiMonWorld::OnButtonYes);
}

void UUiMonWorld::OnButtonYes()
{
	if (TestPlayer)
	{
		if (UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
		{
			TestPlayer->LogItemComp->GetInvenInfo();
			TestPlayer->LogItemComp->GetEquipInfo();

			WorldGi->UserFullInfoGi = TestPlayer->LoginComp->UserFullInfo;
			WorldGi->bGameStart = true;
			WorldGi->bBattleReward = true;

			if (MonWorld && MonWorld->MonsterType == EMonsterType::Monster)
			{
				UGameplayStatics::OpenLevel(TestPlayer,FName("TestMap"));
			}
			else if (MonWorld && MonWorld->MonsterType == EMonsterType::Boss)
			{
				UGameplayStatics::OpenLevel(TestPlayer,FName("Mk_BossMap"));
			}
		}
		// UGameplayStatics::OpenLevel(TestPlayer,FName("TestMap"));
	}
}

void UUiMonWorld::OnButtonNo()
{
}
