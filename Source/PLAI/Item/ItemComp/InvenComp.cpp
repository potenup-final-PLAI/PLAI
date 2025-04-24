// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenComp.h"

#include "JsonObjectConverter.h"
#include "StoreComp.h"
#include "Components/BoxComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"
#include "EnvironmentQuery/EnvQueryDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "PLAI/Item/Item/ItemMaster.h"
#include "PLAI/Item/Npc/NpcStart.h"
#include "PLAI/Item/Npc/NpcStore.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Slot/SlotEquip.h"
#include "PLAI/Item/UI/Inventory/EquipInven/EquipInven.h"
#include "PLAI/Item/UI/Inventory/ItemDetail/ItemDetail.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemGold.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemInven.h"
#include "PLAI/Item/UI/Inventory/StoreInven/StoreInven.h"


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
		MenuInven->AddToViewport();
		MenuInven->WBP_ItemInven->SetVisibility(ESlateVisibility::Hidden);
		MenuInven->WBP_EquipInven->SetVisibility(ESlateVisibility::Hidden);
		MenuInven->WBP_ItemDetail->SetVisibility(ESlateVisibility::Hidden);
	}
}


// Called every frame
void UInvenComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (TestPlayer->IsLocallyControlled() && PC->WasInputKeyJustPressed(EKeys::R)){ CatchItem();}
    //임시함수임
	if (TestPlayer->IsLocallyControlled() && PC->WasInputKeyJustPressed(EKeys::P)){ SetGold(500);}
	
	if (PC && PC->IsLocalController() && PC->WasInputKeyJustPressed(EKeys::Q))
	{   UE_LOG(LogTemp, Warning, TEXT("인벤컴프 Q키 !"));
		ItemMaster = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory,TestPlayer->GetActorLocation() +
		TestPlayer->GetActorForwardVector() * 50,FRotator(0,0,0));
		
		int32 randIndex = FMath::RandRange(0,1);
		ItemMaster->ItemStruct.ItemTop = 0;
		ItemMaster->ItemStruct.ItemIndex = randIndex;
		
		int32 randDetail = FMath::RandRange(0,ItemMaster->ItemParent->ItemStructTop.ItemMeshTops[ItemMaster->ItemStruct.ItemTop].
		ItemMeshIndexes[ItemMaster->ItemStruct.ItemIndex].ItemMeshTypes[ItemMaster->ItemStruct.ItemIndexType].StaticMeshes.Num()-1);
		ItemMaster->ItemStruct.ItemIndexDetail = randDetail;
	}
	
	if (PC->WasInputKeyJustPressed(EKeys::One))
		{  Server_SpawnOneItem(); }
	
	if (PC->WasInputKeyJustPressed(EKeys::Two))
	    { UE_LOG(LogTemp, Warning, TEXT("인벤컴프 Two키 !"));
		ItemMaster = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory,TestPlayer->GetActorLocation() +
			TestPlayer->GetActorForwardVector() * 50,FRotator(0,0,0));
		ItemMaster->ItemStruct.ItemTop = 2;
		int32 randIndex = FMath::RandRange(0,1);
		int32 randDetail = FMath::RandRange(0,ItemMaster->ItemParent->ItemStructTop.ItemMeshTops[ItemMaster->ItemStruct.ItemTop].
		ItemMeshIndexes[ItemMaster->ItemStruct.ItemIndex].ItemMeshTypes[ItemMaster->ItemStruct.ItemIndexType].StaticMeshes.Num()-1);
		
		ItemMaster->ItemStruct.ItemIndex = randIndex;
		ItemMaster->ItemStruct.ItemIndexDetail = randDetail;
	}

	if (PC->WasInputKeyJustPressed(EKeys::Three))
	{
		if (TestPlayer->HasAuthority())
		{ UE_LOG(LogTemp,Warning,TEXT("인벤컴프 %s%s"),*GetOwner()->GetName(),
			TestPlayer->HasAuthority() ? TEXT("서버") : TEXT("클라")); }
	}
	// 아이템 테이블 Test//
	if (PC->WasInputKeyJustPressed(EKeys::Four))
	{
		if (ItemDataTable)
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
			}
		}
	}
	
	if (PC && TestPlayer->IsLocallyControlled() && PC->WasInputKeyJustPressed(EKeys::Nine))
		if (PC && PC->IsLocalController() && PC->WasInputKeyJustPressed(EKeys::Nine))
	{
		LoadItemInventory();
		LoadEquipInventory();
	}
	if (PC && TestPlayer->IsLocallyControlled() && PC->WasInputKeyJustPressed(EKeys::Zero))
	{
		SaveItemInventory();
		SaveEquipInventory();
	}

	// 아이템창 출력
	if (PC && TestPlayer->IsLocallyControlled() && PC->WasInputKeyJustPressed(EKeys::I))
	{ UE_LOG(LogTemp, Warning, TEXT("인벤컴프 I키 !"));
		if (!MenuInven)	{ UE_LOG(LogTemp, Warning, TEXT("인벤컴프 메뉴인벤 없다 ㅠㅠ %s"),
			TestPlayer->HasAuthority() ? TEXT("서버") : TEXT("클라")); return;};
		EnumKey = EEnumKey::Item;
			ItemInvenTory(EnumKey, MenuInven->WBP_ItemInven);
	}
    // 장비창 출력
	if (PC && PC->IsLocalController() && PC->WasInputKeyJustReleased(EKeys::E))
	{ UE_LOG(LogTemp, Warning, TEXT("인벤컴프 E키 "));
		if (!MenuInven)	{ UE_LOG(LogTemp, Warning, TEXT("인벤컴프 메뉴인벤 없다 ㅠㅠ %s"),
			TestPlayer->HasAuthority() ? TEXT("서버") : TEXT("클라")); return;};
		EnumKey = EEnumKey::Equip;
		ItemInvenTory(EnumKey, MenuInven->WBP_EquipInven); }

	if (PC && TestPlayer->IsLocallyControlled() && PC->WasInputKeyJustPressed(EKeys::LeftMouseButton))
	{
		FHitResult Hit;
        PC->GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		if (Hit.bBlockingHit)
		{
			if (ANpcStart* Start = Cast<ANpcStart>(Hit.GetActor()))
			{
				Start->OnNpcStart.BindUObject(this,&UInvenComp::NpcItem);
				Start->WarriorStarter();
				UE_LOG(LogTemp,Warning,TEXT("인벤컴프 왼쪽 마우스버튼 Npc마즘? %s"),*Start->GetName())
			}
			else if (ANpcStore* Store = Cast<ANpcStore>(Hit.GetActor()))
			{
				UE_LOG(LogTemp,Warning,TEXT("인벤컴프 마우스 왼쪽 찍은 엑터 스토어 엑터 찎힘"))
				TestPlayer->StoreComp->SetStoreInven(Store->ItemStructsArray);

				if (FlipflopStore == false)
				{
					TestPlayer->StoreComp->StoreInven->SetVisibility(ESlateVisibility::Visible);
					FlipflopStore = true;
				}
				else
				{
					TestPlayer->StoreComp->StoreInven->SetVisibility(ESlateVisibility::Hidden);
					FlipflopStore = false;
				}
			}
			else
			{
				UE_LOG(LogTemp,Warning,TEXT("인벤컴프 마우스 왼쪽 아무것도 안찍힘"))
			}
		}
	}
	if (PC && TestPlayer->IsLocallyControlled() && PC->WasInputKeyJustPressed(EKeys::RightMouseButton))
	{
		FHitResult Hit;
		PC->GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		if (Hit.bBlockingHit)
		{
			UE_LOG(LogTemp,Warning,TEXT("인벤컴프 마우스 왼쪽 찍은 엑터는? %s"),*Hit.GetActor()->GetName())
			DrawDebugSphere(GetWorld(), Hit.Location, 20, 20, FColor::Red, false,1);
			if (ANpcStart* Start = Cast<ANpcStart>(Hit.GetActor()))
			{
				// 이 딜리게이트는 NpcStart에 있음
				Start->OnNpcStart.BindUObject(this,&UInvenComp::NpcItem);
				Start->HunterStarter();
			}
		}
	}
}

void UInvenComp::SetGold(int32 Getgold)
{
	Gold += Getgold;
	MenuInven->WBP_ItemInven->WbpItemGold->Gold->SetText(FText::AsNumber(Gold));
}

void UInvenComp::Server_SpawnOneItem_Implementation()
{
	if (TestPlayer->IsLocallyControlled())
	{
		ItemMaster = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory,TestPlayer->GetActorLocation() +
			TestPlayer->GetActorForwardVector() * 50,FRotator(0,0,0));
		int32 randIndex = FMath::RandRange(0,4);
		if (!ItemMaster) {UE_LOG(LogTemp, Warning, TEXT("인벤컴프 One키 ItemMaster없음 !"));return;}
		ItemMaster->ItemStruct.ItemTop = 1;
		ItemMaster->ItemStruct.ItemIndex = randIndex;
		
		int32 randDetail = FMath::RandRange(0,ItemMaster->ItemParent->ItemStructTop.ItemMeshTops[ItemMaster->ItemStruct.ItemTop].
		ItemMeshIndexes[ItemMaster->ItemStruct.ItemIndex].ItemMeshTypes[ItemMaster->ItemStruct.ItemIndexType].StaticMeshes.Num()-1);
		ItemMaster->ItemStruct.ItemIndexDetail = randDetail;
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

void UInvenComp::ItemInvenTory(EEnumKey Key, UUserWidget* Inven)
{
	if (Flipflop == false)
	{
		// UE_LOG(LogTemp,Warning,TEXT("UInvenComp::ItemInvenTory() 켯다"));
		Inven->SetVisibility(ESlateVisibility::Visible);
		Flipflop = true;
	}
	else
	{
		// UE_LOG(LogTemp,Warning,TEXT("UInvenComp::ItemInvenTory() 껏다"));
		Inven->SetVisibility(ESlateVisibility::Hidden);
		Flipflop = false;
	}
}

void UInvenComp::Server_GetItem_Implementation(const FItemStruct& ItemStruct)
{
	Client_GetItem(ItemStruct);
}
void UInvenComp::Client_GetItem_Implementation(const FItemStruct& ItemStruct)
{
	GetItem(ItemStruct);
	UE_LOG(LogTemp, Warning, TEXT("Client_GetItem() 실행됨: %s"), GetOwner()->HasAuthority() ? TEXT("서버") : TEXT("클라"));
	// 또는 IsLocallyControlled() 체크
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (PawnOwner && PawnOwner->IsLocallyControlled())
	{
		UE_LOG(LogTemp, Warning, TEXT("이 Client_GetItem은 로컬 클라이언트에서 실행됨"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("이 Client_GetItem은 로컬이 아님!"));
	}
}

void UInvenComp::GetItem(const FItemStruct& ItemStruct)
{
	UE_LOG(LogTemp,Warning,TEXT("UInvenComp::GetItme() %s"),TestPlayer->HasAuthority() ? TEXT("서버") : TEXT("클라"));
	
	bool bSlot = false;
	// if (MenuInven->WBP_ItemInven->WrapBox->GetChildAt(0) == nullptr){return;}
	for (UWidget* Widget : MenuInven->WBP_ItemInven->WrapBox->GetAllChildren())
	{
		USlot* Slot = Cast<USlot>(Widget);
		if (Slot && Slot->ItemStruct.ItemTop == ItemStruct.ItemTop &&Slot->ItemStruct.ItemIndex == ItemStruct.ItemIndex
			&&Slot->ItemStruct.ItemIndexType == ItemStruct.ItemIndexType 
			&&Slot->ItemStruct.ItemIndexDetail == ItemStruct.ItemIndexDetail)
		{
			Slot->ItemStruct.ItemNum++;
			Slot->SlotCountUpdate(Slot->ItemStruct.ItemNum);
			bSlot = true;
			break;
		}
	}
	if (bSlot == false)
	{
		for (UWidget* Widget : MenuInven->WBP_ItemInven->WrapBox->GetAllChildren())
		{
			USlot* Slot = Cast<USlot>(Widget);
			if (Slot->ItemStruct.ItemTop == -1)
			{
				Slot->ItemStruct = ItemStruct;
				Slot->SlotImageUpdate();
				break;
			}
		}
	}
}

void UInvenComp::Server_DestroyItem_Implementation(AItem* Item)
{ Item->Destroy(); }


void UInvenComp::Server_EquipItem_Implementation(const FItemStruct& ItemStruct, EquipSlotType SlotType)
{
	EquipItem(ItemStruct, SlotType);
}

void UInvenComp::EquipItem(const FItemStruct& ItemStruct, EquipSlotType SlotType)
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
		ItemWeapon->AttachToComponent(TestPlayer->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Weapon_R"));
		ItemWeapon->ItemStruct = ItemStruct;
		ItemWeapon->BoxComp->SetSimulatePhysics(ECollisionEnabled::NoCollision);
	}
	else if (SlotType == EquipSlotType::Armor)
	{
		UE_LOG(LogTemp,Warning,TEXT("UInvenComp::EquipSlot타입 %s"),*StaticEnum<EquipSlotType>()->GetNameStringByValue((int8)SlotType));
		ItemArmor = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory,TestPlayer->GetActorLocation() + FVector(0,-100,0),
		FRotator(0,0,0),SpawnParams);
		ItemArmor->AttachToComponent(TestPlayer->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Weapon_L"));
		ItemArmor->ItemStruct = ItemStruct;
		ItemArmor->BoxComp->SetSimulatePhysics(ECollisionEnabled::NoCollision);
	}
	else if (SlotType == EquipSlotType::Helmet)
	{
		UE_LOG(LogTemp,Warning,TEXT("UInvenComp::EquipSlot타입 %s"),*StaticEnum<EquipSlotType>()->GetNameStringByValue((int8)SlotType));
        ItemHelmet = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory,TestPlayer->GetActorLocation() + FVector(0,0,80),FRotator(0,0,0));
		ItemHelmet->AttachToComponent(TestPlayer->GetMesh(),FAttachmentTransformRules::KeepWorldTransform, TEXT("headSocket"));
		ItemHelmet->ItemStruct = ItemStruct;
		ItemHelmet->BoxComp->SetSimulatePhysics(ECollisionEnabled::NoCollision);
		// ItemHelmet->SetActorScale3D(FVector(0.8,0.8,0.8));
	}
	else if (SlotType == EquipSlotType::Gloves)
	{
		UE_LOG(LogTemp,Warning,TEXT("UInvenComp::EquipSlot타입 %s"),*StaticEnum<EquipSlotType>()->GetNameStringByValue((int8)SlotType));
		ItemGlove = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory,TestPlayer->GetActorLocation() + FVector(-100,0,-50),FRotator(0,0,0));
		ItemGlove->AttachToComponent(TestPlayer->GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,("HeadSocket"));
		ItemGlove->BoxComp->SetSimulatePhysics(ECollisionEnabled::NoCollision);
		ItemGlove->ItemStruct = ItemStruct;
		// ItemGlove->SetActorRelativeScale3D(FVector(0.6,0.6,0.6));
		ItemGlove->SetActorRelativeRotation(FRotator(-90,0,0));
	}
	else if (SlotType == EquipSlotType::Boots)
	{
		UE_LOG(LogTemp,Warning,TEXT("UInvenComp::EquipSlot타입 %s"),*StaticEnum<EquipSlotType>()->GetNameStringByValue((int8)SlotType));
		Itemboots = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory,TestPlayer->GetActorLocation() + FVector(75,75,75),FRotator(0,0,0));
		Itemboots->AttachToActor(TestPlayer,FAttachmentTransformRules::KeepWorldTransform);
		Itemboots->BoxComp->SetSimulatePhysics(ECollisionEnabled::NoCollision);
		Itemboots->ItemStruct = ItemStruct;
		// Itemboots->SetActorRelativeScale3D(FVector(0.5,0.5,0.5));
		Itemboots->SetActorRelativeRotation(FRotator(0,-90,0));
		Itemboots->SetActorRelativeLocation(FVector(75,75,75));
	}
}

void UInvenComp::NpcItem(const FItemStructsArray& ItemStructsArray)
{
	for (int32 i = 0; ItemStructsArray.ItemStructs.Num() > i; i++)
	{
		for (int32 e = 0; e < static_cast<int32>(EquipSlotType::Boots); e++)
		{
			StartSlotType = static_cast<EquipSlotType>(e);
			if (ItemStructsArray.ItemStructs[i].ItemIndex == static_cast<int32>(StartSlotType))
			{
				break;
			}
		}
		Server_EquipItem(ItemStructsArray.ItemStructs[i],StartSlotType);
	}
	EquipSetting(ItemStructsArray);
}

void UInvenComp::EquipSetting(const FItemStructsArray& ItemStructsArray)
{
	for (int32 i = 0; ItemStructsArray.ItemStructs.Num() > i; i++)
	{
		for (UWidget* Widget : MenuInven->WBP_EquipInven->LeftBox->GetAllChildren())
		{
			USlotEquip* SlotEquip = Cast<USlotEquip>(Widget);
			if (ItemStructsArray.ItemStructs[i].ItemIndex == static_cast<int32>(SlotEquip->SlotType))
			{
				SlotEquip->ItemStruct = ItemStructsArray.ItemStructs[i];
				SlotEquip->SlotImageUpdate();
			}
		}
	}
	SaveEquipInventory();
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
		{   UE_LOG(LogTemp,Warning,TEXT("인벤컴프 R Hit발생%s"),*Hits[i].GetActor()->GetName())
			AItem* Item = Cast<AItem>(Hits[i].GetActor());
			if (Item)
			{ UE_LOG(LogTemp,Warning,TEXT("인벤컴프 R 아이템먹음"))
				Server_GetItem(Item->ItemStruct); Server_DestroyItem(Item); }
			else{ UE_LOG(LogTemp,Warning,TEXT("인벤컴프 R 아이템못먹음")) }
		}
	}
}



void UInvenComp::SaveItemInventory()
{
	TArray<FItemStruct> ItemStructs;
	
	for (UWidget* Widget : MenuInven->WBP_ItemInven->WrapBox->GetAllChildren())
	{ USlot* Slot = Cast<USlot>(Widget);
		ItemStructs.Add(Slot->ItemStruct); }

	FItemStructsArray ItemStructsArray;
	ItemStructsArray.ItemStructs = ItemStructs;

	FString JsonValue;
	FJsonObjectConverter::UStructToJsonObjectString(ItemStructsArray, JsonValue);
	
	FString path = FString::Printf(TEXT("%s%s"),*FPaths::ProjectDir(),TEXT("Inventory.tst"));
	FFileHelper::SaveStringToFile(JsonValue,*path);
	
	UE_LOG(LogTemp,Warning,TEXT("인벤컴프 아이템창 구조체 제이슨 저장"))
}

void UInvenComp::LoadItemInventory()
{
	FString path = FString::Printf(TEXT("%s%s"), *FPaths::ProjectDir(), TEXT("Inventory.tst"));
	FString JsonString;
	FFileHelper::LoadFileToString(JsonString, *path);

	FItemStructsArray ItemStructsArray;
	FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &ItemStructsArray);
	
	for (int32 i = 0; i < ItemStructsArray.ItemStructs.Num(); i++)
	{
		USlot* Slot = Cast<USlot>(MenuInven->WBP_ItemInven->WrapBox->GetChildAt(i));
		Slot->ItemStruct = ItemStructsArray.ItemStructs[i];
		if (Slot->ItemStruct.ItemTop == -1)
		{ UE_LOG(LogTemp,Warning,TEXT("인벤컴프 슬롯 itemtop -1 리턴")); }
		else
		{ Slot->SlotImageUpdate(); }
	}
	UE_LOG(LogTemp,Warning,TEXT("인벤컴프 아이템창 구조체 제이슨 로드"))
}

void UInvenComp::SaveEquipInventory()
{
	FItemStructsArray ItemStructsArray;
	for (UWidget* Widget : MenuInven->WBP_EquipInven->LeftBox->GetAllChildren())
	{
		USlotEquip* SlotEquip = Cast<USlotEquip>(Widget);
		int32 index = MenuInven->WBP_EquipInven->LeftBox->GetChildIndex(Widget);
		ItemStructsArray.ItemStructs.Add(SlotEquip->ItemStruct);
	}
	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(ItemStructsArray,JsonString);
	UE_LOG(LogTemp,Warning,TEXT("인벤컴프 장비창 구조체 제이슨 저장"))
	
	FString path = FString::Printf(TEXT("%s%s"),*FPaths::ProjectDir(),TEXT("Equip.tst"));
	FFileHelper::SaveStringToFile(JsonString, *path);
}

void UInvenComp::LoadEquipInventory()
{
	FItemStructsArray ItemStructsArray;
	FString JsonString;
	FString path = FString::Printf(TEXT("%s%s"),*FPaths::ProjectDir(),TEXT("Equip.tst"));
	FFileHelper::LoadFileToString(JsonString,*path);
	
	FJsonObjectConverter::JsonObjectStringToUStruct(JsonString,&ItemStructsArray);
	for (UWidget* Widget : MenuInven->WBP_EquipInven->LeftBox->GetAllChildren())
	{
		USlotEquip* SlotEquip = Cast<USlotEquip>(Widget);
		int32 index = MenuInven->WBP_EquipInven->LeftBox->GetChildIndex(Widget);
		SlotEquip->ItemStruct = ItemStructsArray.ItemStructs[index];
		SlotEquip->SlotImageUpdate();
		//ServerRpc로 넘기면 SlotEquip을 바로 넘기는것이 아닌 SlotEquip 정보만 넘김
		Server_EquipItem(ItemStructsArray.ItemStructs[index],SlotEquip->SlotType);
	}
	UE_LOG(LogTemp,Warning,TEXT("인벤컴프 장비창 구조체 제이슨 로드"))
}

void UInvenComp::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInvenComp, ItemMaster);
}

