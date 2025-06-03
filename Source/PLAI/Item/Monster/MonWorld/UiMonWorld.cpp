// Fill out your copyright notice in the Description page of Project Settings.


#include "UiMonWorld.h"

#include "MonWorld.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"
#include "Kismet/GameplayStatics.h"
#include "PLAI/Item/GameInstance/WorldGi.h"
#include "PLAI/Item/ItemComp/CreComp.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/Login/LoginComp.h"
#include "PLAI/Item/Npc/NpcNet.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Inventory/EquipInven/EquipInven.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemInven.h"
#include "PLAI/Item/UI/Slot/SlotEquip.h"

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
			TestPlayer->LogItemComp->GetUserLevel();
			
			WorldGi->UserFullInfoGi = TestPlayer->LoginComp->UserFullInfo;
			WorldGi->bGameStart = true;
			WorldGi->bBattleReward = true;
            WorldGi->MonsterType = MonWorld->MonsterType;
			if (TestPlayer && TestPlayer->CreComp->Creature)
			{ WorldGi->Creature = TestPlayer->CreComp->Creature; }

			if (!TestPlayer->HasAuthority()){UE_LOG(LogTemp,Warning,TEXT("UiMonWorld OnButtonYes TestPlayer 서버가 아님")) return;}
			if (MonWorld && MonWorld->MonsterType == EMonsterType::Monster)
			{
				GetWorld()->ServerTravel(TEXT("/Game/JS/Maps/TestMap?listen"));
			}
			else if (MonWorld && MonWorld->MonsterType == EMonsterType::Boss)
			{
				GetWorld()->ServerTravel(TEXT("/Game/Mk_Item/MK_BossMap?listen"));
			}
			
			if (!TestPlayer->IsLocallyControlled()){UE_LOG(LogTemp,Warning,TEXT("UiMonWorld OnButtonYes 아이템 쉴드 넣기전 IsLocalPlayer 아님")) return;}

			FUserShield UserShield;
			UserShield.UserName = TestPlayer->LoginComp->UserFullInfo.character_info.character_name;
			if (USlotEquip* SlotEquip = Cast<USlotEquip>(TestPlayer->InvenComp->MenuInven->WBP_EquipInven->LeftBox->GetChildAt(3)))
			{
				if (SlotEquip->ItemStructTable.ItemTop != -1)
				{
					UserShield.UserShield = SlotEquip->ItemStructTable.ItemStructStat.item_SHI;
				}
			}
			WorldGi->UserShields.UserShields.Add(UserShield);
		}
	}
	// ChangeNpcPersonality();
}

void UUiMonWorld::OnButtonNo()
{
}

// UE_LOG(LogTemp,Warning,TEXT("UUiMonWorld:: Gi->쉴드값 넣기 닉넴 [%s] 쉴드값 [%d]"),
// 	*WorldGi->UserShields.UserShields[0].UserName,WorldGi->UserShields.UserShields[0].UserShield);

// void UUiMonWorld::ChangeNpcPersonality()
// {
// 	if(UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
// 	{
// 		WorldGi->NpcPersonality = FString(TEXT("승리에 취한"));
// 	}
// }
