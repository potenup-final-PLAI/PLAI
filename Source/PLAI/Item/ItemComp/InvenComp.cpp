// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenComp.h"

#include "JsonObjectConverter.h"
#include "MovieSceneTracksComponentTypes.h"
#include "Components/Image.h"
#include "Components/WrapBox.h"
#include "PLAI/Item/Item/ItemMaster.h"
#include "PLAI/Item/Item/Equip/ItemEquip.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Slot/SlotEquip.h"
#include "PLAI/Item/UI/Inventory/EquipInven/EquipInven.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemInven.h"


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
	
	MenuInven = CreateWidget<UMenuInven>(GetWorld(),MenuInvenFactory);
	MenuInven->AddToViewport();
	MenuInven->WBP_ItemInven->SetVisibility(ESlateVisibility::Hidden);
	MenuInven->WBP_EquipInven->SetVisibility(ESlateVisibility::Hidden);
	
	TestPlayer = Cast<ATestPlayer>(GetOwner());
	PC = Cast<APlayerController>(GetOwner()->GetWorld()->GetFirstPlayerController());

	AItemMaster* Item = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory,FVector(0,0,0),FRotator(0,0,0));
}


// Called every frame
void UInvenComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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
	if (PC && PC->IsLocalController() && PC->WasInputKeyJustPressed(EKeys::One))
	{   UE_LOG(LogTemp, Warning, TEXT("인벤컴프 One키 !"));
		ItemMaster = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory,TestPlayer->GetActorLocation() +
			TestPlayer->GetActorForwardVector() * 50,FRotator(0,0,0));
		int32 randIndex = FMath::RandRange(0,1);
		ItemMaster->ItemStruct.ItemTop = 1;
		ItemMaster->ItemStruct.ItemIndex = randIndex;
		
		int32 randDetail = FMath::RandRange(0,ItemMaster->ItemParent->ItemStructTop.ItemMeshTops[ItemMaster->ItemStruct.ItemTop].
		ItemMeshIndexes[ItemMaster->ItemStruct.ItemIndex].ItemMeshTypes[ItemMaster->ItemStruct.ItemIndexType].StaticMeshes.Num()-1);
		ItemMaster->ItemStruct.ItemIndexDetail = randDetail;
	}
	if (PC && PC->IsLocalController() && PC->WasInputKeyJustPressed(EKeys::Two))
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
	if (PC && PC->IsLocalController() && PC->WasInputKeyJustPressed(EKeys::Zero))
	{
		SaveItemInventory();
	}
	if (PC && PC->IsLocalController() && PC->WasInputKeyJustPressed(EKeys::Nine))
	{
		LoadItemInventory();
	}

	// 아이템창 출력
	
	
	if (PC && PC->IsLocalController() && PC->WasInputKeyJustPressed(EKeys::I))
	{ UE_LOG(LogTemp, Warning, TEXT("인벤컴프 I키 !"));
		EnumKey = EEnumKey::Item;
			ItemInvenTory(EnumKey, MenuInven->WBP_ItemInven);
	}
    // 장비창 출력
	if (PC && PC->IsLocalController() && PC->WasInputKeyJustReleased(EKeys::E))
	{ UE_LOG(LogTemp, Warning, TEXT("인벤컴프 E키 "));
		EnumKey = EEnumKey::Equip;
		ItemInvenTory(EnumKey, MenuInven->WBP_EquipInven); }
}

void UInvenComp::ItemInvenTory(EEnumKey Key, UUserWidget* Inven)
{
	if (Flipflop == false)
	{
		UE_LOG(LogTemp,Warning,TEXT("UInvenComp::ItemInvenTory() 켯다"));
		Inven->SetVisibility(ESlateVisibility::Visible);
		Flipflop = true;
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("UInvenComp::ItemInvenTory() 껏다"));
		Inven->SetVisibility(ESlateVisibility::Hidden);
		Flipflop = false;
	}
}

void UInvenComp::GetItem(const FItemStruct& ItemStruct)
{
	UE_LOG(LogTemp,Warning,TEXT("UInvenComp::GetItme()"));
	
	bool bSlot = false;
	if (MenuInven->WBP_ItemInven->WrapBox->GetChildAt(0) == nullptr) return;
	for (UWidget* Widget : MenuInven->WBP_ItemInven->WrapBox->GetAllChildren())
	{
		USlot* Slot = Cast<USlot>(Widget);
		if (Slot && Slot->ItemStruct.ItemTop == ItemStruct.ItemTop &&Slot->ItemStruct.ItemIndex == ItemStruct.ItemIndex
			&&Slot->ItemStruct.ItemIndexType == ItemStruct.ItemIndexType 
			&&Slot->ItemStruct.ItemIndexDetail == ItemStruct.ItemIndexDetail)
		{
			Slot->ItemStruct.ItemNum++;
			Slot->SlotCountUpdate(Slot->ItemStruct.ItemNum);
			UE_LOG(LogTemp,Warning,TEXT("UInvenComp::슬롯갯수 증가"));
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
				UE_LOG(LogTemp,Warning,TEXT("UInvenComp::슬롯추가 증가"));
				Slot->ItemStruct = ItemStruct;
				Slot->SlotImageUpdate();
				break;
			}
		}
	}
}
void UInvenComp::EquipItem(const FItemStruct& ItemStruct, USlotEquip* Equip)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = GetOwner();
	if (Equip->SlotType == EquipSlotType::Weapon)
	{
		if (ItemWeapon != nullptr) return;
		UE_LOG(LogTemp,Warning,TEXT("UInvenComp::EquipSlot타입 %s"),*StaticEnum<EquipSlotType>()->GetNameStringByValue((int8)Equip->SlotType));
		ItemWeapon = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory,TestPlayer->GetActorLocation() + FVector(0,100,0),
		FRotator(0,0,0),SpawnParams);
		ItemWeapon->AttachToComponent(TestPlayer->GetMesh(), FAttachmentTransformRules::KeepWorldTransform, TEXT("Weapon_R"));
		ItemWeapon->ItemStruct = ItemStruct;
	}
	else if (Equip->SlotType == EquipSlotType::Armor)
	{
		UE_LOG(LogTemp,Warning,TEXT("UInvenComp::EquipSlot타입 %s"),*StaticEnum<EquipSlotType>()->GetNameStringByValue((int8)Equip->SlotType));
		// if (ItemArmor != nullptr) return;
		ItemArmor = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory,TestPlayer->GetActorLocation() + FVector(0,-100,0),
		FRotator(0,0,0),SpawnParams);
		ItemArmor->AttachToComponent(TestPlayer->GetMesh(), FAttachmentTransformRules::KeepWorldTransform, TEXT("Weapon_L"));
		ItemArmor->ItemStruct = ItemStruct;
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
	
	FString path = FString::Printf(TEXT("%s%s"),*FPaths::ProjectDir(),TEXT("AllOktest.tst"));
	FFileHelper::SaveStringToFile(JsonValue,*path);
	
	UE_LOG(LogTemp,Warning,TEXT("인벤컴프 json string%s"),*JsonValue);
}

void UInvenComp::LoadItemInventory()
{
	FString path = FString::Printf(TEXT("%s%s"), *FPaths::ProjectDir(), TEXT("AllOktest.tst"));
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
}

void UInvenComp::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
	
	

