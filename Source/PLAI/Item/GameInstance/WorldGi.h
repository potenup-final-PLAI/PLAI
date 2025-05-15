// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PLAI/Item/Item/ItemMaster.h"
#include "PLAI/Item/Login/UserStruct.h"
#include "WorldGi.generated.h"

/**
 * 
 */
UCLASS(Blueprintable,Blueprintable)
class PLAI_API UWorldGi : public UGameInstance
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	TArray<int32>bWorldSpawnInt;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	bool bGameStart = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	bool bBattleReward = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	FUserFullInfo UserFullInfoGi;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	FUserFullInfo UserFullInfoGiStat;

    UPROPERTY(EditAnywhere)
	TSubclassOf<AItemMaster> ItemMasterFactory;

	UPROPERTY(EditAnywhere)
	class AItemMaster* ItemMaster;

	UPROPERTY(EditAnywhere)
	UDataTable* ItemDataTable;
	
	UPROPERTY(EditAnywhere)
	bool bLoginMe = false;

	void EquipActor(AActor* MyActor);
};
