// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "PLAI/Item/Item/ItemMaster.h"
#include "PLAI/Item/Login/UserStruct.h"
#include "WorldGi.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FUserShield
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FString UserName = FString("UserName");

	UPROPERTY(EditAnywhere)
	int32 UserShield = 0;
};

USTRUCT(BlueprintType)
struct FUserShields
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<FUserShield>UserShields;
};

UCLASS(Blueprintable,Blueprintable)
class PLAI_API UWorldGi : public UGameInstance
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void CreateSession(FString displayName, int32 playerCount);
	void OnCreateSessionComplete(FName sessionName, bool success);

	IOnlineSessionPtr SessionInterface;
	
	virtual void Init() override;
	
	UPROPERTY(EditAnywhere)
	FUserShields UserShields;
	
	// UPROPERTY(EditAnywhere)
	// FUserShield UserShield;
	
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
