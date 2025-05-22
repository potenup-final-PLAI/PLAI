// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LogItemComp.h"
#include "UObject/Object.h"
#include "UserStruct.generated.h"




class ULogItemComp;

USTRUCT(BlueprintType,Blueprintable)
struct FMaterialBase : public FTableRowBase
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere)
    class UMaterialInterface* Material;

    UPROPERTY(EditAnywhere)
    int32 Index = 0;

    UPROPERTY(EditAnywhere)
    class UStaticMesh* StaticMesh;
};

USTRUCT(BlueprintType)
struct FPosition
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float x = 0.0f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float y= 0.0f;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float z= 0.0f;
};


USTRUCT(BlueprintType)
struct FPutUserLevel
{
    GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere)
    FString character_id = FString("char");
    UPROPERTY(EditAnywhere)
    int level = 0;
    UPROPERTY(EditAnywhere)
    int current_exp = 0;
    UPROPERTY(EditAnywhere)
    int max_exp = 0;
    UPROPERTY(EditAnywhere)
    TArray<FString>traits;
    UPROPERTY(EditAnywhere)
    FPosition position;
};

USTRUCT(BlueprintType)
struct Fstats
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    int32 hp = 100;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    int32 attack = 10;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    int32 defense = 10;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    int32 resistance = 0;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float critical_rate = 10;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    float critical_damage = 10;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    int32 move_range = 0;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    int32 speed = 0;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    int32 points = 0;
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
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    FString item_id;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    int32 item_category;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    int32 item_type;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    int32 item_class;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    FString item_name;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    FString category_name;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    FString description;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    int32 level;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    int32 price;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    Foptions options;

    // inventory_info에만 존재
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    int32 counts = 0; // 기본값 0
};

USTRUCT(BlueprintType)
struct FcharInfo
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    FString character_id = FString("Init Character Id");
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    FString character_name = FString("Init Character Name");
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    FString job;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    FString gender;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    TArray<FString> traits;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    int32 level;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    int32 current_exp;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    int32 max_exp;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    FPosition position;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    Fstats stats;
};

USTRUCT(BlueprintType)
struct FEquipmentInfo
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    TArray<FitemInfo> item_list;
};

USTRUCT(BlueprintType)
struct FInventoryInfo
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    TArray<FitemInfo> item_list;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    int32 gold;
};

USTRUCT(BlueprintType)
struct FUserFullInfo
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    FString message;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    FString user_id = FString("Init UserId");
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    FcharInfo character_info;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    FEquipmentInfo equipment_info;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
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

USTRUCT(BlueprintType)
struct FLevelInfo : public FTableRowBase
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere)
    int32 level = 1;
    UPROPERTY(EditAnywhere)
    int32 Exp = 0;
};

