// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemComp.h"
#include "Components/ActorComponent.h"
#include "PLAI/Item/Item/ItemStruct.h"
#include "PLAI/Item/Item/Equip//ItemEquip.h"
#include "PLAI/Item/UI/Inventory/MenuInven.h"
#include "PLAI/Item/UI/Slot/SlotEquip.h"
#include "InvenComp.generated.h"

class APlayerController;
class ATestPlayer;

UENUM(BlueprintType)
enum class EEnumKey : uint8
{
	Item,
	Equip
};

USTRUCT(BlueprintType,Blueprintable)
struct FItemStructsArray
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FItemStruct>ItemStructs;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAI_API UInvenComp : public UItemComp
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInvenComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
public:
	EEnumKey EnumKey;

	EquipSlotType StartSlotType;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMenuInven> MenuInvenFactory;
	
	UPROPERTY(EditAnywhere)
	class UMenuInven* MenuInven;
public:
    UPROPERTY(EditAnywhere)
	ATestPlayer* TestPlayer;

	UPROPERTY(EditAnywhere)
	APlayerController* PC;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AItemEquip>EquipFactory;
	UPROPERTY(EditAnywhere)
	class AItemEquip* ItemEquip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AItemMaster> ItemMasterFactory;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	class AItemMaster* ItemMaster;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AItemMaster* ItemWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AItemMaster* ItemArmor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AItemMaster* ItemHelmet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AItemMaster* ItemGlove;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AItemMaster* Itemboots;

	UFUNCTION(Server,Reliable)
	void Server_UnEquip(EquipSlotType SlotType);

	UFUNCTION(Server,Reliable)
	void Server_SpawnOneItem();

	UFUNCTION(Server,Reliable)
	void Server_DestroyItem(AItem* Item);
	
	void ItemInvenTory(EEnumKey key, UUserWidget* Inven);

	UFUNCTION(Server,Reliable)
	void Server_GetItem(const FItemStruct& ItemStruct);
	UFUNCTION(Client,Reliable)
	void Client_GetItem(const FItemStruct& ItemStruct);
	
	void GetItem(const FItemStruct& ItemStruct);
	
	UFUNCTION(Server,Reliable) // NetMulticast는 안써도됨 = 아이템이 자동으로 동기화중
 	void Server_EquipItem(const FItemStruct& ItemStruct, EquipSlotType SlotType);

	void EquipItem(const FItemStruct& ItemStruct, EquipSlotType SlotType);

	UFUNCTION()
	void NpcItem(const FItemStructsArray& ItemStructsArray);

	void CatchItem();
	
	void EquipSetting(const FItemStructsArray& ItemStructsArray);
	
	void InventorySetting();

	void SaveItemInventory();
	void LoadItemInventory();

	void SaveEquipInventory();
	void LoadEquipInventory();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
public:
	UPROPERTY(EditAnywhere)
	bool Flipflop = false;
};
