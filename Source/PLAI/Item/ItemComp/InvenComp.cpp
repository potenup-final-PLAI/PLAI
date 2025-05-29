// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenComp.h"
#include "JsonObjectConverter.h"
#include "StoreComp.h"
#include "Components/BoxComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PLAI/Item/Creture/Creature.h"
#include "PLAI/Item/GameInstance/WorldGi.h"
#include "PLAI/Item/Item/ItemMaster.h"
#include "PLAI/Item/Login/LoginComp.h"
#include "PLAI/Item/Npc/NpcStart.h"
#include "PLAI/Item/Npc/NpcStore.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Character/UiChaLevelUp.h"
#include "PLAI/Item/UI/Character/UIChaStat.h"
#include "PLAI/Item/UI/Slot/SlotEquip.h"
#include "PLAI/Item/UI/Inventory/EquipInven/EquipInven.h"
#include "PLAI/Item/UI/Inventory/InputUi/InputUi.h"
#include "PLAI/Item/UI/Inventory/ItemDetail/ItemDetail.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemGold.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemInven.h"
#include "PLAI/Item/UI/Inventory/QuickInven/QuickInven.h"
#include "PLAI/Item/UI/Inventory/StoreInven/StoreInven.h"
#include "PLAI/Item/UI/Inventory/UiCre/UiCre.h"
#include "PLAI/Item/UI/Main/UiChaView.h"
#include "PLAI/Item/UI/Slot/SlotCre.h"


// Sets default values for this component's properties
UInvenComp::UInvenComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ..
}


// Called when the game starts
void UInvenComp::BeginPlay()
{
	Super::BeginPlay();
	TestPlayer = Cast<ATestPlayer>(GetOwner());
	PC = Cast<APlayerController>(GetOwner()->GetWorld()->GetFirstPlayerController());
	
	if (TestPlayer->IsLocallyControlled())
	{
		MenuInven = CreateWidget<UMenuInven>(GetWorld(),MenuInvenFactory);
		if (UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
		{
			if (WorldGi->bBattleReward == true || WorldGi->bGameStart == true)
			{
				if (!MenuInven){MenuInven = CreateWidget<UMenuInven>(GetWorld(),MenuInvenFactory);}
				MenuInven->AddToViewport(1);
				MenuInven->WBP_EquipInven->SetVisibility(ESlateVisibility::Hidden);
				MenuInven->WBP_ItemInven->SetVisibility(ESlateVisibility::Hidden);
				MenuInven->WBP_ItemDetail->SetVisibility(ESlateVisibility::Hidden);
				MenuInven->Wbp_UIChaStat->SetVisibility(ESlateVisibility::Hidden);
				MenuInven->WBP_InputUi->SetVisibility(ESlateVisibility::Hidden);
				MenuInven->Wbp_UiChaLevelUp->SetVisibility(ESlateVisibility::Hidden);
				MenuInven->WBP_QuickInven->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

// Called every frame
void UInvenComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    // 데이터테이블 템 먹기
	if (TestPlayer->IsLocallyControlled() && PC->WasInputKeyJustPressed(EKeys::R)){ CatchItem();}
	
	if (TestPlayer->HasAuthority() && PC->WasInputKeyJustPressed(EKeys::Four))
	{
		Server_SpawnOneItem();
	}
	
	if (TestPlayer->IsLocallyControlled() && ItemDataTable && PC->WasInputKeyJustPressed(EKeys::Five))
	{
		Server_SpawnCreature();
	}

	if (TestPlayer->IsLocallyControlled() && ItemDataTable && PC->WasInputKeyJustPressed(EKeys::Six))
	{
		SetGold(100000);
	}

	
	if (PC && TestPlayer->IsLocallyControlled() && PC->WasInputKeyJustPressed(EKeys::LeftMouseButton))
	{
		FHitResult Hit;
        PC->GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		if (Hit.bBlockingHit)
		{
			if (ANpcStart* Start = Cast<ANpcStart>(Hit.GetActor()))
			{
				if (FlipflopStart == false)
				{
					Start->UiNpcStart->SetVisibility(ESlateVisibility::Visible);
					FlipflopStart = true;
					if (bStartEquip == true)
					{
						Start->UiNpcStart->StartJobText->SetText(FText::FromString(FString(TEXT("이미 장비를 지급해 드렸습니다"))));
						Start->UiNpcStart->TextA->SetVisibility(ESlateVisibility::Hidden);
						Start->UiNpcStart->TextB->SetVisibility(ESlateVisibility::Hidden);
					}
					if (bStartEquip == false)
					{
						bStartEquip = true;
						Start->UiNpcStart->StartJobText->SetText(FText::FromString(TestPlayer->LoginComp->UserFullInfo.character_info.job));
						FlipflopStart = true;
						if (TestPlayer->LoginComp->UserFullInfo.character_info.job == FString("warrior"))
						{
							Start->OnNpcStart.BindUObject(this,&UInvenComp::NpcItem);
							Start->WarriorStarter();
						}
						else if (TestPlayer->LoginComp->UserFullInfo.character_info.job == FString("archer"))
						{
							Start->OnNpcStart.BindUObject(this,&UInvenComp::NpcItem);
							Start->HunterStarter();
						}
					}
				}
			}
			if (ANpcStore* Store = Cast<ANpcStore>(Hit.GetActor()))
			{
				if (FlipflopStore == false)
				{
					Store->StoreInven->SetVisibility(ESlateVisibility::Visible);
					FlipflopStore = true;
				}
				else
				{
					Store->StoreInven->SetVisibility(ESlateVisibility::Hidden);
					FlipflopStore = false;
				}
			}
		}
	}
}

void UInvenComp::Server_SpawnCreature_Implementation()
{
	Client_GetCreature();
}

void UInvenComp::Client_GetCreature_Implementation()
{
	TArray<FName> RawNames = ItemDataTable->GetRowNames();
	if (RawNames.IsValidIndex(33))
	{ UE_LOG(LogTemp,Warning,TEXT("InvenComp 소환수 소환 5번키 입력"))
		FItemStructTable* ItemStructTable = ItemDataTable->FindRow<FItemStructTable>(RawNames[33],TEXT("InvenComp100"));
		TestPlayer->InvenComp->MenuInven->WBP_SlotCre->SpawnCreature(*ItemStructTable,TestPlayer); }
	else
	{ UE_LOG(LogTemp,Warning,TEXT("InvenComp 소환수 소환 5번키 입력 없는데?")) }
}

void UInvenComp::SetGold(int32 Getgold)
{
	if (TestPlayer->IsLocallyControlled())
	{
		TestPlayer->LoginComp->UserFullInfo.inventory_info.gold += Getgold;
		MenuInven->WBP_ItemInven->WbpItemGold->Gold->SetText(FText::AsNumber(TestPlayer->LoginComp->UserFullInfo.inventory_info.gold));
		MenuInven->Wbp_ItemGold->Gold->SetText(FText::AsNumber(TestPlayer->LoginComp->UserFullInfo.inventory_info.gold));
	}
}

void UInvenComp::Server_SpawnOneItem_Implementation()
{
    if (TestPlayer->IsLocallyControlled() && ItemDataTable)
    {
    	TArray<FName> RawNames = ItemDataTable->GetRowNames();
    	int32 Rand = FMath::RandRange(0,RawNames.Num()-1);
    	if (RawNames.IsValidIndex(Rand))
    	{
    		FItemStructTable* ItemStructTable = ItemDataTable->FindRow<FItemStructTable>(RawNames[Rand],TEXT("InvenComp100"));
    		ItemMaster = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory);
    		ItemMaster->SetActorLocation(TestPlayer->GetActorLocation() + TestPlayer->GetActorForwardVector() *75);
    		ItemMaster->ItemStructTable = *ItemStructTable;
    		ItemMaster->StaticMesh->SetStaticMesh(ItemStructTable->StaticMesh);
    		ItemMaster->StaticMesh->SetStaticMesh(ItemMaster->ItemStructTable.StaticMesh);
    	}
    }
}

void UInvenComp::Server_UnEquip_Implementation(EquipSlotType SlotType)
{
	if (SlotType == EquipSlotType::Armor)
	{
		if (!ItemArmor) return;
		ItemArmor->Destroy();
		ItemArmor = nullptr;
	}
	if (SlotType == EquipSlotType::Boots)
	{
		if (!Itemboots) return;
		Itemboots->Destroy();
		Itemboots = nullptr;
	}
	if (SlotType == EquipSlotType::Gloves)
	{
		if (!ItemGlove) return;
		ItemGlove->Destroy();
		ItemGlove = nullptr;
	}
	if (SlotType == EquipSlotType::Helmet)
	{
		if (!ItemHelmet) return;
		ItemHelmet->Destroy();
		ItemHelmet = nullptr;
	}
	if (SlotType == EquipSlotType::Weapon)
	{
		if (!ItemWeapon) return;
		ItemWeapon->Destroy();
		ItemWeapon = nullptr;
	}
}

void UInvenComp::Server_GetItem_Implementation(const FItemStructTable& ItemStructTable)
{
	Client_GetItem(ItemStructTable);
}
void UInvenComp::Client_GetItem_Implementation(const FItemStructTable& ItemStructTable)
{
	GetItem(ItemStructTable);
	UE_LOG(LogTemp, Warning, TEXT("Client_GetItem() 실행됨: %s"), GetOwner()->HasAuthority() ? TEXT("서버") : TEXT("클라"));
	// 또는 IsLocallyControlled() 체크
	APawn* PawnOwner = Cast<APawn>(GetOwner());
}

void UInvenComp::GetItem(const FItemStructTable& ItemStructTable)
{
	UE_LOG(LogTemp,Warning,TEXT("UInvenComp::GetItme() %s"),TestPlayer->HasAuthority() ? TEXT("서버") : TEXT("클라"));
	
	bool bSlot = false;
	// if (MenuInven->WBP_ItemInven->WrapBox->GetChildAt(0) == nullptr){return;}
	for (UWidget* Widget : MenuInven->WBP_ItemInven->WrapBox->GetAllChildren())
	{
		USlot* Slot = Cast<USlot>(Widget);
		if (Slot && Slot->ItemStructTable.ItemTop != 1 && Slot->ItemStructTable.ItemTop == ItemStructTable.ItemTop &&Slot->ItemStructTable.ItemIndex == ItemStructTable.ItemIndex
			&&Slot->ItemStructTable.ItemIndexType == ItemStructTable.ItemIndexType 
			&&Slot->ItemStructTable.ItemIndexDetail == ItemStructTable.ItemIndexDetail)
		{
			Slot->ItemStructTable.ItemNum++;
			Slot->SlotCountUpdate(Slot->ItemStructTable.ItemNum);
			bSlot = true;
			break;
		}
	}
	if (bSlot == false)
	{
		for (UWidget* Widget : MenuInven->WBP_ItemInven->WrapBox->GetAllChildren())
		{
			USlot* Slot = Cast<USlot>(Widget);
			if (Slot->ItemStructTable.ItemTop == -1)
			{
				Slot->ItemStructTable = ItemStructTable;
				Slot->SlotImageUpdate();
				break;
			}
		}
	}
}

void UInvenComp::Server_DestroyItem_Implementation(AItem* Item)
{
	if (!IsValid(Item)) return;
	Item->Destroy();
}

void UInvenComp::Server_EquipItem_Implementation(const FItemStructTable& ItemStructTable, EquipSlotType SlotType)
{
	EquipItem(ItemStructTable, SlotType);
}

void UInvenComp::EquipItem(const FItemStructTable& ItemStructTable, EquipSlotType SlotType)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = GetOwner();
	
	if (SlotType == EquipSlotType::Weapon)
	{
		if (ItemWeapon != nullptr) return;
		UE_LOG(LogTemp,Warning,TEXT("UInvenComp::EquipSlot타입 %s"),*StaticEnum<EquipSlotType>()->GetNameStringByValue((int8)SlotType));
		ItemWeapon = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory,TestPlayer->GetActorLocation() + FVector(0,100,0),
		FRotator(0,0,0),SpawnParams);
		ItemWeapon->AttachToComponent(TestPlayer->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("hand_rSocket"));
		ItemWeapon->ItemStructTable = ItemStructTable;
		ItemWeapon->StaticMesh->SetStaticMesh(ItemStructTable.StaticMesh);
		ItemWeapon->BoxComp->SetSimulatePhysics(ECollisionEnabled::NoCollision);
		
		ItemWeapon->AddActorLocalOffset(FVector(-20, -10, 60));
		ItemWeapon->AddActorLocalRotation(FRotator(-165, 10, 100));

		if (ItemStructTable.Material)
		{ ItemWeapon->StaticMesh->SetMaterial(0,ItemStructTable.Material);}
	}
	else if (SlotType == EquipSlotType::Armor)
	{
		ItemArmor = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory);
		ItemArmor->AttachToComponent(TestPlayer->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Chest"));
		ItemArmor->ItemStructTable = ItemStructTable;
		ItemArmor->StaticMesh->SetStaticMesh(ItemStructTable.StaticMesh);
		ItemArmor->BoxComp->SetSimulatePhysics(ECollisionEnabled::NoCollision);

	    ItemArmor->AddActorLocalOffset(FVector(-20, 0, -80));
		ItemArmor->AddActorLocalRotation(FRotator(0, 90, 0));
	    ItemArmor->SetActorRelativeScale3D(FVector(1.05, 1.05, 1.05));
		
		if (ItemStructTable.Material)
		{ ItemArmor->StaticMesh->SetMaterial(0,ItemStructTable.Material);}
	}
	else if (SlotType == EquipSlotType::Helmet)
	{
        ItemHelmet = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory,TestPlayer->GetActorLocation() + FVector(0,0,80),FRotator(0,0,0));
		ItemHelmet->AttachToComponent(TestPlayer->GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("headSocket"));
		ItemHelmet->ItemStructTable = ItemStructTable;
		ItemHelmet->BoxComp->SetSimulatePhysics(ECollisionEnabled::NoCollision);
		ItemHelmet->StaticMesh->SetStaticMesh(ItemStructTable.StaticMesh);
		
		ItemHelmet->AddActorWorldOffset(FVector(5, 0, 10));
		ItemHelmet->AddActorLocalRotation(FRotator(0, -90, 90));
		ItemHelmet->SetActorRelativeScale3D(FVector(0.8,0.8,0.8));

		if (ItemStructTable.Material)
		{ ItemHelmet->StaticMesh->SetMaterial(0,ItemStructTable.Material);}
	}
	else if (SlotType == EquipSlotType::Gloves)
	{
		ItemGlove = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory,TestPlayer->GetActorLocation() + FVector(-100,0,-50),FRotator(0,0,0));
		ItemGlove->AttachToComponent(TestPlayer->GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,("HeadSocket"));
		ItemGlove->BoxComp->SetSimulatePhysics(ECollisionEnabled::NoCollision);
		ItemGlove->ItemStructTable = ItemStructTable;
		ItemGlove->SetActorRelativeRotation(FRotator(-90,0,0));
		ItemGlove->AddActorLocalOffset(FVector(0,0,-150));
		ItemGlove->StaticMesh->SetStaticMesh(ItemStructTable.StaticMesh);
		
		if (ItemStructTable.Material)
		{ ItemGlove->StaticMesh->SetMaterial(0,ItemStructTable.Material);}
	}
	else if (SlotType == EquipSlotType::Boots)
	{
		Itemboots = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory,TestPlayer->GetActorLocation() + FVector(75,75,75),FRotator(0,0,0));
		Itemboots->AttachToComponent(TestPlayer->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("FX_Ult_Passive"));
		Itemboots->BoxComp->SetSimulatePhysics(ECollisionEnabled::NoCollision);
		Itemboots->ItemStructTable = ItemStructTable;
		Itemboots->StaticMesh->SetStaticMesh(ItemStructTable.StaticMesh);
		
		Itemboots->AddActorWorldOffset(FVector(-0, 0, -130));
		Itemboots->AddActorLocalRotation(FRotator(0, 100, 0));
		
		if (ItemStructTable.Material)
		{ Itemboots->StaticMesh->SetMaterial(0,ItemStructTable.Material);}
	}
	
	if (UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
	{
		if( WorldGi->bLoginMe == false)
		{
			UE_LOG(LogTemp,Warning,TEXT("InvenComp 아직 로그인 안됨 bLoginMe머고 %d"),WorldGi->bLoginMe) return;
		};
	};

	if (!TestPlayer->IsLocallyControlled()) return;
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
	{
		FitemInfo ItemInfo;
		TestPlayer->LoginComp->UserFullInfo.equipment_info.item_list.Empty();
		for (UWidget* widget : TestPlayer->InvenComp->MenuInven->WBP_EquipInven->LeftBox->GetAllChildren())
		{
			if (USlotEquip* SlotEquip = Cast<USlotEquip>(widget))
			{
				if (SlotEquip->ItemStructTable.ItemTop == -1)
				{
					UE_LOG(LogTemp,Warning,TEXT("LogItemComp GetEquipInfo 장비슬롯 있음 인덱스 무엇?%d"),
						TestPlayer->InvenComp->MenuInven->WBP_EquipInven->LeftBox->GetChildIndex(SlotEquip))
					continue;
				}
				ItemInfo.options.attack = SlotEquip->ItemStructTable.ItemStructStat.item_ATK;
				ItemInfo.options.hp = SlotEquip->ItemStructTable.ItemStructStat.item_SHI;
				ItemInfo.options.defense = SlotEquip->ItemStructTable.ItemStructStat.item_DEF;
				ItemInfo.options.resistance = SlotEquip->ItemStructTable.ItemStructStat.item_RES;
				ItemInfo.options.critical_rate = SlotEquip->ItemStructTable.ItemStructStat.Item_CRIT;
				ItemInfo.options.critical_damage =SlotEquip->ItemStructTable.ItemStructStat.item_CRITDMG;
				ItemInfo.item_id = SlotEquip->ItemStructTable.Item_Id;
				ItemInfo.item_name = SlotEquip->ItemStructTable.Name;
				ItemInfo.price = SlotEquip->ItemStructTable.ItemGold;
				TestPlayer->LoginComp->UserFullInfo.equipment_info.item_list.Add(ItemInfo);
			}
		}
		TestPlayer->InvenComp->MenuInven->Wbp_UIChaStat->SetUiChaStat(&TestPlayer->LoginComp->UserFullInfo);
		
		for (int32 i = 0; i < TestPlayer->LoginComp->UserFullInfo.equipment_info.item_list.Num(); i++)
		{ // UE_LOG(LogTemp,Warning,TEXT("InvenCOmp 497번쨰줄 SetUiChaStat에 들어가는 장비정보 [%s]"),
			// 	*TestPlayer->LoginComp->UserFullInfo.equipment_info.item_list[i].item_name);
		} },1,false);
}

void UInvenComp::NpcItem(const FItemStructTables ItemStructTables)
{
	for (int32 i = 0; ItemStructTables.ItemStructTables.Num() > i; i++)
	{
		for (int32 e = 0; e < static_cast<int32>(EquipSlotType::Boots); e++)
		{
			StartSlotType = static_cast<EquipSlotType>(e);
			if (ItemStructTables.ItemStructTables[i].ItemIndex == static_cast<int32>(StartSlotType))
			{
				break;
			}
		}
		Server_EquipItem(ItemStructTables.ItemStructTables[i],StartSlotType);
	}
	EquipSetting(ItemStructTables);
}

void UInvenComp::EquipSetting(const FItemStructTables& ItemStructTables)
{
	if (!TestPlayer->IsLocallyControlled())
	{
		return;
	}
	for (int32 i = 0; ItemStructTables.ItemStructTables.Num() > i; i++)
	{
		for (UWidget* Widget : MenuInven->WBP_EquipInven->LeftBox->GetAllChildren())
		{
			USlotEquip* SlotEquip = Cast<USlotEquip>(Widget);
			if (ItemStructTables.ItemStructTables[i].ItemIndex == static_cast<int32>(SlotEquip->SlotType))
			{
				SlotEquip->ItemStructTable = ItemStructTables.ItemStructTables[i];
				SlotEquip->SlotImageUpdate();
			}
		}
	}
}

void UInvenComp::CatchItem()
{
	TArray<FHitResult> Hits;
	FCollisionQueryParams Params;
	FVector Loc = TestPlayer->GetActorLocation();
	DrawDebugBox(GetWorld(),Loc+TestPlayer->GetActorForwardVector()*250,FVector(100,100,100),FColor::Red,false, 1.0f);
	bool hitinfo = GetWorld()->SweepMultiByChannel(Hits,Loc + TestPlayer->GetActorForwardVector() * 250,Loc + TestPlayer->GetActorForwardVector() * 250,
		FQuat::Identity,ECC_Visibility,FCollisionShape::MakeBox(FVector(100,100,100)),Params);
	if (hitinfo)
	{
		for (int32 i = 0; i < Hits.Num(); i++)
		{  
			AItem* Item = Cast<AItem>(Hits[i].GetActor());
			if (Item)
			{
				Server_GetItem(Item->ItemStructTable); Server_DestroyItem(Item);
			}
		}
	}
}

void UInvenComp::SaveItemInventory()
{
	FItemStructTables ItemStructTables;

	for (UWidget* Widget : MenuInven->WBP_ItemInven->WrapBox->GetAllChildren())
	{ USlot* Slot = Cast<USlot>(Widget);
		ItemStructTables.ItemStructTables.Add(Slot->ItemStructTable); }
	
	FString JsonValue;
	FJsonObjectConverter::UStructToJsonObjectString(ItemStructTables, JsonValue);
	
	FString path = FString::Printf(TEXT("%s%s"),*FPaths::ProjectDir(),TEXT("Inventory.tst"));
	FFileHelper::SaveStringToFile(JsonValue,*path);
	
	// UE_LOG(LogTemp,Warning,TEXT("인벤컴프 아이템창 구조체 제이슨 저장"))
}

void UInvenComp::LoadItemInventory()
{
	FString path = FString::Printf(TEXT("%s%s"), *FPaths::ProjectDir(), TEXT("Inventory.tst"));
	FString JsonString;
	FFileHelper::LoadFileToString(JsonString, *path);

	FItemStructTables ItemStructTables;
	FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &ItemStructTables);
	
	for (int32 i = 0; i < ItemStructTables.ItemStructTables.Num(); i++)
	{
		USlot* Slot = Cast<USlot>(MenuInven->WBP_ItemInven->WrapBox->GetChildAt(i));
		Slot->ItemStructTable = ItemStructTables.ItemStructTables[i];
		Slot->SlotImageUpdate();
	}
	// UE_LOG(LogTemp,Warning,TEXT("인벤컴프 아이템창 구조체 제이슨 로드"))
}

void UInvenComp::SaveEquipInventory()
{
	FItemStructTables ItemStructTables;
	for (UWidget* Widget : MenuInven->WBP_EquipInven->LeftBox->GetAllChildren())
	{
		USlotEquip* SlotEquip = Cast<USlotEquip>(Widget);
		int32 index = MenuInven->WBP_EquipInven->LeftBox->GetChildIndex(Widget);
		ItemStructTables.ItemStructTables.Add(SlotEquip->ItemStructTable);
	}
	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(ItemStructTables,JsonString);
	// UE_LOG(LogTemp,Warning,TEXT("인벤컴프 장비창 구조체 제이슨 저장"))
	
	FString path = FString::Printf(TEXT("%s%s"),*FPaths::ProjectDir(),TEXT("Equip.tst"));
	FFileHelper::SaveStringToFile(JsonString, *path);
}

void UInvenComp::LoadEquipInventory()
{
	FItemStructTables ItemStructTables;
	FString JsonString;
	FString path = FString::Printf(TEXT("%s%s"),*FPaths::ProjectDir(),TEXT("Equip.tst"));
	FFileHelper::LoadFileToString(JsonString,*path);
	
	FJsonObjectConverter::JsonObjectStringToUStruct(JsonString,&ItemStructTables);
	for (UWidget* Widget : MenuInven->WBP_EquipInven->LeftBox->GetAllChildren())
	{
		USlotEquip* SlotEquip = Cast<USlotEquip>(Widget);
		int32 index = MenuInven->WBP_EquipInven->LeftBox->GetChildIndex(Widget);
		SlotEquip->ItemStructTable = ItemStructTables.ItemStructTables[index];
		SlotEquip->SlotImageUpdate();
		//ServerRpc로 넘기면 SlotEquip을 바로 넘기는것이 아닌 SlotEquip 정보만 넘김
		Server_EquipItem(ItemStructTables.ItemStructTables[index],SlotEquip->SlotType);
	}
	UE_LOG(LogTemp,Warning,TEXT("인벤컴프 장비창 구조체 제이슨 로드"))
}

void UInvenComp::TurnReward()
{
	if(UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
	{
		if (WorldGi->MonsterType == EMonsterType::Monster)
		{
			UiTurnReward = CreateWidget<class UUiTurnReward>(GetWorld(),UUiTurnRewardFactory);
			UiTurnReward->AddToViewport();

			int32 Exp = FMath::RandRange(7300,8000);
			TestPlayer->InvenComp->GetExp(Exp);

			UiTurnReward->RewardExp->SetText(FText::AsNumber(Exp));
	
			int32 RandGold = FMath::RandRange(100000,250000);
			UiTurnReward->RewardGold->SetText(FText::AsNumber(RandGold));
			SetGold(RandGold);
	
			int32 RandomReward = FMath::RandRange(2,4);
			for (int32 i = 0; i < RandomReward; i++)
			{
				TArray<FName>Raws = ItemDataTable->GetRowNames();
				int32 index = FMath::RandRange(0,Raws.Num()-1);
				FName ItemName = Raws[index];
				FItemStructTable* ItemStructTable = ItemDataTable->FindRow<FItemStructTable>(ItemName,TEXT("InvecComp548"));
	
				UiTurnReward->UiTurnRewardImage = CreateWidget<UUiTurnRewardImage>(GetWorld(),UiTurnReward->UiTurnRewardImageFactory);
				FSlateBrush Brush;
				Brush.SetResourceObject(ItemStructTable->Texture);
				UiTurnReward->UiTurnRewardImage->RewardImage->SetBrush(Brush);
				UiTurnReward->RewardBox->AddChildToWrapBox(UiTurnReward->UiTurnRewardImage);
	
				Server_GetItem(*ItemStructTable);
			}
			FTimerHandle TurnRewardTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TurnRewardTimerHandle,[this, RandGold]()
			{
				if (UiTurnReward)
				{
					TestPlayer->LoginComp->UserFullInfo.inventory_info.gold += RandGold;
					TestPlayer->LogItemComp->GetEquipInfo();
					TestPlayer->LogItemComp->GetInvenInfo();
			
					UiTurnReward->RemoveFromParent();
					UE_LOG(LogTemp,Warning,TEXT("InvenComp 턴제 리워드 1.5초뒤 UI 삭제"))
				}
			},1.5f,false);
		}
		else
		{
			UiTurnReward = CreateWidget<class UUiTurnReward>(GetWorld(),UUiTurnRewardFactory);
			UiTurnReward->AddToViewport();

			int32 Exp = FMath::RandRange(50000,60000);
			TestPlayer->InvenComp->GetExp(Exp);

			UiTurnReward->RewardExp->SetText(FText::AsNumber(Exp));
	
			int32 RandGold = FMath::RandRange(10000,15000);
			UiTurnReward->RewardGold->SetText(FText::AsNumber(RandGold));
			SetGold(RandGold);

			TArray<FName>Raws = ItemDataTable->GetRowNames();
			for (int i = 0; i < 2; i++)
			{
				if (34 + i > Raws.Num()-1)return;
				FName ItemLegend = Raws[34 + i];
				FItemStructTable* ItemStructTable = ItemDataTable->FindRow<FItemStructTable>(ItemLegend,TEXT("InvecComp618"));
				UiTurnReward->UiTurnRewardImage = CreateWidget<UUiTurnRewardImage>(GetWorld(),UiTurnReward->UiTurnRewardImageFactory);
				FSlateBrush Brush;
				Brush.SetResourceObject(ItemStructTable->Texture);
				UiTurnReward->UiTurnRewardImage->RewardImage->SetBrush(Brush);
				UiTurnReward->RewardBox->AddChildToWrapBox(UiTurnReward->UiTurnRewardImage);
				Server_GetItem(*ItemStructTable);
			}
			FTimerHandle TurnRewardTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TurnRewardTimerHandle,[this, RandGold]()
			{
				if (UiTurnReward)
				{
					TestPlayer->LoginComp->UserFullInfo.inventory_info.gold += RandGold;
					TestPlayer->LogItemComp->GetEquipInfo();
					TestPlayer->LogItemComp->GetInvenInfo();
			
					UiTurnReward->RemoveFromParent();
					UE_LOG(LogTemp,Warning,TEXT("InvenComp 턴제 리워드 1.5초뒤 UI 삭제"))
				}
			},1.5f,false);
		}
	}
}

void UInvenComp::GetExp(int32 Exp)
{
	TestPlayer->LoginComp->UserFullInfo.character_info.current_exp += Exp;
	MenuInven->Wbp_ChaView->SetUiChaView(TestPlayer->LoginComp->UserFullInfo);

	GetLevel();
}

void UInvenComp::GetLevel()
{
	TArray<int32>LevelCounts;
	TArray<FName>Levels = TestPlayer->LoginComp->LevelTable->GetRowNames();
	for (FName Level : Levels)
	{
		FName NextLevelName = FName(*FString::FromInt(TestPlayer->LoginComp->UserFullInfo.character_info.level+1));
		
		FLevelInfo* LevelStruct = TestPlayer->LoginComp->LevelTable->FindRow<FLevelInfo>(Level,TEXT("InvenComp"));
		FLevelInfo* NextLevelStruct = TestPlayer->LoginComp->LevelTable->FindRow<FLevelInfo>(NextLevelName,TEXT("InvenComp"));

		UE_LOG(LogTemp,Warning,TEXT("InvenComp 레벨업 경치 들어오는중? 테이블 도는중 레벨은? [%d]"),LevelStruct->level)

		if (TestPlayer->LoginComp->UserFullInfo.character_info.current_exp < LevelStruct->Exp)
		{
			UE_LOG(LogTemp,Warning,TEXT("InvenComp GetLevel에서 넘기는 레벨 배열 크기는? [%d]"),Levels.Num())
			
			LevelCounts.Add(LevelStruct->level);
			UiGetLevel(LevelCounts);
			return;
		}
		if (TestPlayer->LoginComp->UserFullInfo.character_info.level == LevelStruct->level &&
			TestPlayer->LoginComp->UserFullInfo.character_info.current_exp >= LevelStruct->Exp)
		{
			LevelCounts.Add(LevelStruct->level);
			UE_LOG(LogTemp,Warning,TEXT("InvenComp GetLevel에서 레벨스에 담기는 LevelStruct->level 값은? [%d]"),LevelStruct->level)
			TestPlayer->LoginComp->UserFullInfo.character_info.current_exp -= LevelStruct->Exp;
			TestPlayer->LoginComp->UserFullInfo.character_info.max_exp = NextLevelStruct->Exp;
			
			TestPlayer->LoginComp->UserFullInfo.character_info.level += 1;
			TestPlayer->InvenComp->MenuInven->Wbp_ChaView->SetUiChaView(TestPlayer->LoginComp->UserFullInfo);
			TestPlayer->InvenComp->MenuInven->Wbp_UIChaStat->SetUiChaStat(&TestPlayer->LoginComp->UserFullInfo);
		}
	}
	GetLevel();
}

void UInvenComp::UiGetLevel(TArray<int32>Levels)
{
	float Time = 0.f;
	UE_LOG(LogTemp,Warning,TEXT("InvenComp 레벨업 UI 떠야하는디 타이머 전에서 Levels 배열갯수는? [%d]"),Levels.Num())
	
	GetWorld()->GetTimerManager().SetTimer(LevelTimerHandle,[this, Time, Levels]()mutable{
		Time += 0.04f;
		MenuInven->Wbp_UiChaLevelUp->SetOpacity(Time);
		if (Time >= 1.f)
		{
			UE_LOG(LogTemp,Warning,TEXT("InvenComp 레벨업 UI 떠야하는디 Levels 배열갯수는? [%d]"),Levels.Num())
			if (Levels.Num() == 0)
			{
				GetWorld()->GetTimerManager().ClearTimer(LevelTimerHandle);
				MenuInven->Wbp_UiChaLevelUp->SetVisibility(ESlateVisibility::Hidden);
				return;
			}
			if ( MenuInven->Wbp_UiChaLevelUp->GetVisibility() == ESlateVisibility::Hidden)
			{ MenuInven->Wbp_UiChaLevelUp->SetVisibility(ESlateVisibility::Visible); }
			MenuInven->Wbp_UiChaLevelUp->LevelUpCount->SetText(FText::AsNumber(Levels[0]));
			Levels.RemoveAt(0);
			Time -= 1.f;
		}
	},0.02f,true);
}

void UInvenComp::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInvenComp, ItemMaster);
}





// if (PC && TestPlayer->IsLocallyControlled() && PC->WasInputKeyJustPressed(EKeys:: MouseButton))
// {
// 	FHitResult Hit;
// 	PC->GetHitResultUnderCursor(ECC_Visibility, true, Hit);
// 	if (Hit.bBlockingHit)
// 	{
// 		// UE_LOG(LogTemp,Warning,TEXT("인벤컴프 마우스 왼쪽 찍은 엑터는? %s"),*Hit.GetActor()->GetName())
// 		DrawDebugSphere(GetWorld(), Hit.Location, 20, 20, FColor::Red, false,1);
// 		if (ANpcStart* Start = Cast<ANpcStart>(Hit.GetActor()))
// 		{
// 			// 이 딜리게이트는 NpcStart에 있음
// 			Start->OnNpcStart.BindUObject(this,&UInvenComp::NpcItem);
// 			Start->HunterStarter();
// 		}
// 	}
// }


// TestPlayer->StoreComp->SetStoreInven(Store->ItemTable);
//
// if (FlipflopStore == false)
// {
// 	TestPlayer->StoreComp->StoreInven->SetVisibility(ESlateVisibility::Visible);
// 	FlipflopStore = true;
// }
// else
// {
// 	TestPlayer->StoreComp->StoreInven->SetVisibility(ESlateVisibility::Hidden);
// 	FlipflopStore = false;
// }

// 인벤토리 창열기

// void UInvenComp::ItemInvenTory(EEnumKey Key, UUserWidget* Inven)
// {
// 	if (Flipflop == false)
// 	{
// 		Inven->SetVisibility(ESlateVisibility::Visible);
// 		Flipflop = true;
// 	}
// 	else
// 	{
// 		Inven->SetVisibility(ESlateVisibility::Hidden);
// 		Flipflop = false;
// 	}
// }

// 장비창 / 인벤토리 열기 함수 구현

// // 아이템창 출력
// if (PC && TestPlayer->IsLocallyControlled() && PC->WasInputKeyJustPressed(EKeys::I))
// { UE_LOG(LogTemp, Warning, TEXT("인벤컴프 I키 !"));
// 	if (!MenuInven)	{ UE_LOG(LogTemp, Warning, TEXT("인벤컴프 메뉴인벤 없다 ㅠㅠ %s"),
// 		TestPlayer->HasAuthority() ? TEXT("서버") : TEXT("클라")); return;};
// 	EnumKey = EEnumKey::Item;
// 		ItemInvenTory(EnumKey, MenuInven->WBP_ItemInven);
// }
//    // 장비창 출력
// if (PC && PC->IsLocalController() && PC->WasInputKeyJustReleased(EKeys::E))
// { UE_LOG(LogTemp, Warning, TEXT("인벤컴프 E키 "));
// 	if (!MenuInven)	{ UE_LOG(LogTemp, Warning, TEXT("인벤컴프 메뉴인벤 없다 ㅠㅠ %s"),
// 		TestPlayer->HasAuthority() ? TEXT("서버") : TEXT("클라")); return;};
// 	EnumKey = EEnumKey::Equip;
// 	ItemInvenTory(EnumKey, MenuInven->WBP_EquipInven); }

// 몬스터 트리거

// 	if (PC && PC->IsLocalController() && PC->WasInputKeyJustPressed(EKeys::Q))
// 	{   UE_LOG(LogTemp, Warning, TEXT("몬스터 트리거 Q키 !"));
//
// 		TArray<FOverlapResult> Hits;
// 		FCollisionQueryParams Params;
// 		Params.AddIgnoredActor(TestPlayer);
//
// 		DrawDebugSphere(GetWorld(),TestPlayer->GetActorLocation()+TestPlayer->GetActorForwardVector() * 50,100,
// 			20,FColor::Red,false,2)
// ;		bool bHit = GetWorld()->OverlapMultiByChannel(Hits,TestPlayer->GetActorLocation()+TestPlayer->GetActorForwardVector() * 50,
// 	FQuat::Identity, ECC_Visibility,FCollisionShape::MakeSphere(100),Params);
// 		
// 		if (bHit)
// 		{
// 			for (FOverlapResult Hit: Hits)
// 			{
// 				if (AMonWorld* MonWorld = Cast<AMonWorld>(Hit.GetActor()))
// 				{
// 					UE_LOG(LogTemp,Warning,TEXT("InvenComp 몬월드 맞노"))
// 					if (UWorldGi* Gi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
// 					{
// 						UGameplayStatics::OpenLevel(TestPlayer,FName("TestMap"));
// 					};
// 				}
// 				else
// 				{
// 					UE_LOG(LogTemp,Warning,TEXT("InvenComp 몬월드 아니야"))
// 				}
// 			}
// 		}
// 	}

// if (PC && TestPlayer->IsLocallyControlled() && PC->WasInputKeyJustPressed(EKeys::Nine))
// {
// 	LoadItemInventory();
// 	LoadEquipInventory();
// }
// if (PC && TestPlayer->IsLocallyControlled() && PC->WasInputKeyJustPressed(EKeys::Zero))
// {
// 	SaveItemInventory();
// 	SaveEquipInventory();
// }

//    // 스팀 세션 만들기
// if (TestPlayer->HasAuthority() && PC->WasInputKeyJustPressed(EKeys::One))
// {
// 	if (UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
// 	{
// 		WorldGi->CreateSession(FString("Wanted"),4);
// 	}
// }
// // 스팀 세션 찾기
// if (TestPlayer->HasAuthority() && PC->WasInputKeyJustPressed(EKeys::Two))
// {
// 	if (UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
// 	{
// 		WorldGi->FindOtherSession();
// 	}
// }
//
// // 스팀 세션 참여하기 
// if (TestPlayer->HasAuthority() && PC->WasInputKeyJustPressed(EKeys::Three))
// {
// 	if (UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
// 	{
// 		WorldGi->JoinOtherSession();
// 	}
// }