// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "PLAI/Item/Item/ItemMaster.h"
#include "PLAI/Item/Login/UserStruct.h"
#include "PLAI/Item/Monster/MonWorld/MonWorld.h"
#include "WorldGi.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnFindSession, FString)

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
	FOnFindSession OnFindSession;

	// 세션 생성
	UFUNCTION()
	void CreateSession(FString displayName, int32 playerCount);
	void OnCreateSessionComplete(FName sessionName, bool success);

	// 세션 조회
	UFUNCTION()
	void FindOtherSession();
	void OnFindSessionComplete(bool bWasSuccessful);

	// 세션 참여
	UFUNCTION()
	void JoinOtherSession();
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);

public:
	// 세션 모든 처리를 진행
	IOnlineSessionPtr SessionInterface;

	// 세션 검색할 때 쓰는 객체
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
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

	UPROPERTY(EditAnywhere)
	bool bLoginMe = false;

	UPROPERTY(EditAnywhere)
	class ACreature* Creature;

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
	EMonsterType MonsterType;

	UPROPERTY(EditAnywhere)
	bool bFirstQuest = false;
	UPROPERTY(EditAnywhere)
	bool bFourthQuest = false;

	UPROPERTY(EditAnywhere)
	FString NpcPersonality = FString(TEXT("NpcPersonality"));

	void EquipActor(AActor* MyActor);
	
};
