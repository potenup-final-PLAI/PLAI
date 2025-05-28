// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NpcCharacter.h"
#include "GameFramework/Character.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/UI/Inventory/StoreInven/StoreInven.h"
#include "NpcStore.generated.h"

UCLASS()
class PLAI_API ANpcStore : public ANpcCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANpcStore();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UDataTable* ItemTable;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AItem> ItemFactory;

	UPROPERTY(EditAnywhere)
	AItem* Item;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UStoreInven>StoreInvenFactory;
	
	UPROPERTY(EditAnywhere)
	class UStoreInven* StoreInven;
	
	// void SetStoreInven();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
