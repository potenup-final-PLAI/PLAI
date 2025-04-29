// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LogItemComp.h"
#include "UObject/Object.h"
#include "UserStruct.generated.h"

USTRUCT(BlueprintType)
struct FPosition
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    float x = 0.0f;
    UPROPERTY(BlueprintReadWrite)
    float y= 0.0f;
    UPROPERTY(BlueprintReadWrite)
    float z= 0.0f;
};

USTRUCT(BlueprintType)
struct Fstats
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    int32 hp;
    UPROPERTY(BlueprintReadWrite)
    int32 attack;
    UPROPERTY(BlueprintReadWrite)
    int32 defense;
    UPROPERTY(BlueprintReadWrite)
    int32 resistance;
    UPROPERTY(BlueprintReadWrite)
    float critical_rate;
    UPROPERTY(BlueprintReadWrite)
    float critical_damage;
    UPROPERTY(BlueprintReadWrite)
    int32 move_range;
    UPROPERTY(BlueprintReadWrite)
    int32 speed;
    UPROPERTY(BlueprintReadWrite)
    int32 points;
};

// USTRUCT(BlueprintType) 얘네는 아이템 인벤 컴프? 장비 보내는 곳에서 정의해놨음
// struct FOptions
// {
//     GENERATED_BODY()
// public:
//     UPROPERTY(BlueprintReadWrite)
//     int32 hp;
//     UPROPERTY(BlueprintReadWrite)
//     int32 attack;
//     UPROPERTY(BlueprintReadWrite)
//     int32 defense;
//     UPROPERTY(BlueprintReadWrite)
//     int32 resistance;
//     UPROPERTY(BlueprintReadWrite)
//     float critical_rate;
//     UPROPERTY(BlueprintReadWrite)
//     float critical_damage;
//     UPROPERTY(BlueprintReadWrite)
//     int32 move_range;
//     UPROPERTY(BlueprintReadWrite)
//     int32 speed;
// };

USTRUCT(BlueprintType)
struct FitemInfo
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    FString item_id;
    UPROPERTY(BlueprintReadWrite)
    int32 item_category;
    UPROPERTY(BlueprintReadWrite)
    int32 item_type;
    UPROPERTY(BlueprintReadWrite)
    int32 item_class;
    UPROPERTY(BlueprintReadWrite)
    FString item_name;
    UPROPERTY(BlueprintReadWrite)
    FString category_name;
    UPROPERTY(BlueprintReadWrite)
    FString description;
    UPROPERTY(BlueprintReadWrite)
    int32 level;
    UPROPERTY(BlueprintReadWrite)
    int32 price;
    UPROPERTY(BlueprintReadWrite)
    Foptions options;

    // inventory_info에만 존재
    UPROPERTY(BlueprintReadWrite)
    int32 counts = 0; // 기본값 0
};

USTRUCT(BlueprintType)
struct FcharInfo
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    FString character_id;
    UPROPERTY(BlueprintReadWrite)
    FString character_name;
    UPROPERTY(BlueprintReadWrite)
    FString job;
    UPROPERTY(BlueprintReadWrite)
    FString gender;
    UPROPERTY(BlueprintReadWrite)
    TArray<FString> traits;
    UPROPERTY(BlueprintReadWrite)
    int32 level;
    UPROPERTY(BlueprintReadWrite)
    int32 current_exp;
    UPROPERTY(BlueprintReadWrite)
    int32 max_exp;
    UPROPERTY(BlueprintReadWrite)
    FPosition position;
    UPROPERTY(BlueprintReadWrite)
    Fstats stats;
};

USTRUCT(BlueprintType)
struct FEquipmentInfo
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    TArray<FitemInfo> item_list;
};

USTRUCT(BlueprintType)
struct FInventoryInfo
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    TArray<FitemInfo> item_list;
    UPROPERTY(BlueprintReadWrite)
    int32 gold;
};

USTRUCT(BlueprintType)
struct FUserFullInfo
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    FString message;
    UPROPERTY(BlueprintReadWrite)
    FString user_id;
    UPROPERTY(BlueprintReadWrite)
    FcharInfo character_info;
    UPROPERTY(BlueprintReadWrite)
    FEquipmentInfo equipment_info;
    UPROPERTY(BlueprintReadWrite)
    FInventoryInfo inventory_info;
};


/**
 * 
 */
UCLASS()
class PLAI_API UUserStruct : public UObject
{
	GENERATED_BODY()
};

