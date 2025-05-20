// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "ItemComp.h"
#include "Components/ActorComponent.h"
#include "PLAI/Item/Item/ItemStruct.h"
#include "PLAI/Item/Item/Equip//ItemEquip.h"
#include "PLAI/Item/UI/Inventory/MenuInven.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemGold.h"
#include "PLAI/Item/UI/TurnReward/UiTurnReward.h"
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
	EEnumKey EnumKey;

    void SetGold(int32 Getgold);

	UPROPERTY(EditAnywhere)
	bool bStartEquip = false;
	
	EquipSlotType StartSlotType;

	UPROPERTY(EditAnywhere)
	class ANpcCharacter* NpcCharacter;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMenuInven> MenuInvenFactory;

	UPROPERTY(EditAnywhere)
	class UDataTable* ItemDataTable;
	
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
	
	// void ItemInvenTory(EEnumKey key, UUserWidget* Inven);

	UFUNCTION(Server,Reliable)
	void Server_GetItem(const FItemStructTable& ItemStructTable);
	UFUNCTION(Client,Reliable)
	void Client_GetItem(const FItemStructTable& ItemStructTable);
	
	void GetItem(const FItemStructTable& ItemStructTable);
	
	UFUNCTION(Server,Reliable) // NetMulticast는 안써도됨 = 아이템이 자동으로 동기화중
 	void Server_EquipItem(const FItemStructTable& ItemStructTable, EquipSlotType SlotType);
	
	void EquipItem(const FItemStructTable& ItemStructTable, EquipSlotType SlotType);

	UFUNCTION(Server,Reliable)
	void Server_SpawnCreature();

	UFUNCTION(Client,Reliable)
	void Client_GetCreature();
	

	UFUNCTION()
	void NpcItem(const FItemStructTables ItemStructTables);

	void CatchItem();
	
	void EquipSetting(const FItemStructTables& ItemStructTables);
	
	void SaveItemInventory();
	void LoadItemInventory();

	void SaveEquipInventory();
	void LoadEquipInventory();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUiTurnReward> UUiTurnRewardFactory;
	
	UPROPERTY(EditAnywhere)
	class UUiTurnReward* UiTurnReward;
	
	void TurnReward();

    void GetExp(int32 Exp);
	void GetLevel();
	void UiGetLevel(TArray<int32>Levels);
	FTimerHandle LevelTimerHandle;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
public:
	UPROPERTY(EditAnywhere)
	bool Flipflop = false;
	bool FlipflopStore = false;

	UPROPERTY(EditAnywhere)
	bool FlipflopStart = false;
};
