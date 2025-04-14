// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemComp.h"
#include "Components/ActorComponent.h"
#include "PLAI/Item/Item/ItemStruct.h"
#include "PLAI/Item/UI/Inventory/MenuInven.h"
#include "InvenComp.generated.h"

class APlayerController;
class ATestPlayer;

UENUM(BlueprintType)
enum class EEnumKey : uint8
{
	Item,
	Equip
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
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMenuInven> MenuInvenFactory;
	
	UPROPERTY(EditAnywhere)
	class UMenuInven* MenuInven;
public:
    UPROPERTY(EditAnywhere)
	ATestPlayer* TestPlayer;

	UPROPERTY(EditAnywhere)
	FItemStruct ItemStruct;

	UPROPERTY(EditAnywhere)
	APlayerController* PC;
	
	void ItemInvenTory(EEnumKey key, UUserWidget* Inven);

public:
	UPROPERTY(EditAnywhere)
	bool Flipflop = false;
};
