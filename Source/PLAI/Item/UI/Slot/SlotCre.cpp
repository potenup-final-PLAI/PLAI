// Fill out your copyright notice in the Description page of Project Settings.


#include "SlotCre.h"

#include "Blueprint/DragDropOperation.h"
#include "PLAI/Item/Creture/Creature.h"
#include "PLAI/Item/Creture/CreFsm.h"
#include "PLAI/Item/Item/ItemObject.h"
#include "PLAI/Item/ItemComp/CreComp.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Inventory/UiCre/UiCre.h"

FReply USlotCre::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (ItemStructTable.CreatureFactory != nullptr)
	{
		if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
		{
			if (TestPlayer->CreComp->Creature == nullptr)
			{
				UE_LOG(LogTemp,Warning,TEXT("slotcre cre없음"));
				return FReply::Handled();
			}
			else
			{
				TestPlayer->CreComp->Creature->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this, TestPlayer]()
				{
					TestPlayer->CreComp->Creature->Destroy(); TestPlayer->CreComp->Creature=nullptr;
				},2,false);
			}
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("USlotCre:: CretureFactory 없음"));
		}
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

bool USlotCre::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                            UDragDropOperation* InOperation)
{
	if (ItemStructTable.ItemTop != -1)
	{
		UE_LOG(LogTemp,Warning,TEXT("USlotCre:: ItemTop != -1 아이템 있음")); return false;
	}
	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
	if (ItemObject && ItemObject->ItemStructTable.CreatureFactory != nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("USlotCre::크리처 들어왔음 NativeOnDrop"));
	}
	Swap(ItemStructTable, ItemObject->ItemStructTable);
	SlotImageUpdate();
	SlotCountUpdate(ItemStructTable.ItemNum);

	if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		TestPlayer->InvenComp->Server_SpawnCreature();
	}


	
	// if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	// {
	// 	FActorSpawnParameters SpawnParams;
	// 	SpawnParams.bNoFail = true; // 실패하지 않게 설정
	// 	SpawnParams.bDeferConstruction = true;
	// 	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	// 	ACreature* Creature = GetWorld()->SpawnActor<ACreature>(ItemStructTable.CreatureFactory,SpawnParams);
	// 	Creature->FinishSpawning(FTransform(TestPlayer->GetActorLocation() + FVector(0,0,500)));
	// 	TestPlayer->CreComp->EquipCreature(Creature);
	// }
	// else
	// {
	// 	UE_LOG(LogTemp,Warning,TEXT("USlotCre:: TestPlayer캐스팅 실패 NativeOnDrop"));
	// }
	//
	// UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnDrop"));
	
	return true;
	
	// return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void USlotCre::SpawnCreature(FItemStructTable ItemStructTab, ATestPlayer* TestPlayer)
{
	SlotImageUpdate();
	SlotCountUpdate(ItemStructTab.ItemNum);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.bNoFail = true; // 실패하지 않게 설정
	SpawnParams.bDeferConstruction = true;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ACreature* Creature = GetWorld()->SpawnActor<ACreature>(ItemStructTab.CreatureFactory,SpawnParams);
	Creature->FinishSpawning(FTransform(TestPlayer->GetActorLocation() + FVector(0,0,500)));
	TestPlayer->CreComp->EquipCreature(Creature);
	Creature->CreFsm->TestPlayer = TestPlayer;
	Creature->ItemStructTable = ItemStructTable;
		
	Creature->CreFsm->CreStruct.Name = ItemStructTable.Name;
	Creature->CreFsm->CreStruct.Atk = ItemStructTable.ItemStructStat.item_ATK;
	Creature->CreFsm->CreStruct.Crit = ItemStructTable.ItemStructStat.Item_CRIT;
	Creature->CreFsm->CreStruct.CritDmg = ItemStructTable.ItemStructStat.item_CRITDMG;
		
	Creature->CreFsm->CreStruct.MaxHp = ItemStructTable.ItemStructStat.item_SHI;
	Creature->CreFsm->CreStruct.CurrentHp = ItemStructTable.ItemStructStat.item_SHI;
	Creature->CreFsm->CreStruct.Def = ItemStructTable.ItemStructStat.item_DEF;
		
	MenuInven->Wbp_UiCre->SetUiCre(&Creature->CreFsm->CreStruct);
}

void USlotCre::SetCreatureStat()
{
}


// if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
// {
// 	if (!TestPlayer->IsLocallyControlled())return;
// 	FActorSpawnParameters SpawnParams;
// 	SpawnParams.bNoFail = true; // 실패하지 않게 설정
// 	SpawnParams.bDeferConstruction = true;
// 	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
// 	ACreature* Creature = GetWorld()->SpawnActor<ACreature>(ItemStructTab.CreatureFactory,SpawnParams);
// 	Creature->FinishSpawning(FTransform(TestPlayer->GetActorLocation() + FVector(0,0,500)));
// 	TestPlayer->CreComp->EquipCreature(Creature);
// 	Creature->ItemStructTable = ItemStructTable;
// 	
// 	Creature->CreFsm->CreStruct.Name = ItemStructTable.Name;
// 	Creature->CreFsm->CreStruct.Atk = ItemStructTable.ItemStructStat.item_ATK;
// 	Creature->CreFsm->CreStruct.Crit = ItemStructTable.ItemStructStat.Item_CRIT;
// 	Creature->CreFsm->CreStruct.CritDmg = ItemStructTable.ItemStructStat.item_CRITDMG;
// 	
// 	Creature->CreFsm->CreStruct.MaxHp = ItemStructTable.ItemStructStat.item_SHI;
// 	Creature->CreFsm->CreStruct.CurrentHp = ItemStructTable.ItemStructStat.item_SHI;
//
// 	Creature->CreFsm->CreStruct.Def = ItemStructTable.ItemStructStat.item_DEF;
// 	// 왜 경험치 1000 초기화 안됨?
// 	// Creature->CreFsm->CreStruct.MaxExp = 1000;
// 	
// 	MenuInven->Wbp_UiCre->SetUiCre(&Creature->CreFsm->CreStruct);
// }
// else
// {
// 	UE_LOG(LogTemp,Warning,TEXT("USlotCre:: TestPlayer캐스팅 실패 NativeOnDrop"));
// }