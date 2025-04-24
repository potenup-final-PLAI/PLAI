// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BattleTypes.generated.h"

//-------Battle Start 할 때 넘겨줘야 할 데이터 구조체-------
USTRUCT(BlueprintType)
struct FCharacterDetail
{
	GENERATED_BODY()

	UPROPERTY()
	FString id;

	UPROPERTY()
	FString name;

	UPROPERTY()
	FString type;

	UPROPERTY()
	FString personality;

	UPROPERTY()
	TArray<FString> skills;
};

USTRUCT(BlueprintType)
struct FEnvironmentState
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FCharacterDetail> characters;

	UPROPERTY()
	FString terrain;

	UPROPERTY()
	FString weather;
};

//-------AI 행동 판단을 위한 데이터 구조체-------
USTRUCT(BlueprintType)
struct FCharacterInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString id;

	UPROPERTY()
	TArray<int32> position;

	UPROPERTY()
	int32 hp;

	UPROPERTY()
	int32 ap;

	UPROPERTY()
	TArray<FString> status_effects;
};

USTRUCT(BlueprintType)
struct FBattleState
{
	GENERATED_BODY()

	UPROPERTY()
	int32 cycle;

	UPROPERTY()
	int32 turn;

	UPROPERTY()
	FString target_monster_id;

	UPROPERTY()
	TArray<FCharacterInfo> characters;
};

USTRUCT(BlueprintType)
struct FBaseStatus
{
	GENERATED_BODY()

	UPROPERTY()
	int32 hp;
	UPROPERTY()
	int32 attack;
	UPROPERTY()
	int32 defense;
	UPROPERTY()
	int32 resistance;
	UPROPERTY()
	float critical_Rate;
	UPROPERTY()
	float critical_Damage;
	UPROPERTY()
	int32 move_Range;
	UPROPERTY()
	int32 speed;
	UPROPERTY()
	int32 points;
};

/**
 * 
 */
UCLASS()
class PLAI_API UBattleTypes : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};
