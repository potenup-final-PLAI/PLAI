// Fill out your copyright notice in the Description page of Project Settings.


#include "CreFsm.h"

#include "Components/TextBlock.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/Login/LoginComp.h"
#include "PLAI/Item/Monster/Monster.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemInven.h"
#include "PLAI/Item/UI/Inventory/UiCre/UiCre.h"


// Sets default values for this component's properties
UCreFsm::UCreFsm()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCreFsm::BeginPlay()
{
	Super::BeginPlay();
	// ...
}


// Called every frame
void UCreFsm::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCreFsm::SetCreStat()
{
	if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		if (CreStruct.CurrentExp > CreStruct.MaxExp)
		{
			CreStruct.MaxExp = CreStruct.MaxExp * 1.2;
			CreStruct.CurrentExp = 0;
			CreStruct.Level++;
		}
		TestPlayer->InvenComp->MenuInven->Wbp_UiCre->SetUiCre(&CreStruct);
	}
}

void UCreFsm::GetMonGold(AMonster* Monster)
{
	if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		int32 GetGold = TestPlayer->LoginComp->UserFullInfo.inventory_info.gold += Monster->MonsterStruct.gold;
		TestPlayer->InvenComp->MenuInven->WBP_ItemInven->WbpItemGold->Gold->SetText(FText::AsNumber(GetGold));
		TestPlayer->InvenComp->MenuInven->Wbp_ItemGold->Gold->SetText(FText::AsNumber(GetGold));
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("CreFsm TestPlayer 캐스팅 실패"))
	}
}
