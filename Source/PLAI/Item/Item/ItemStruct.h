// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "ItemStruct.generated.h"

UENUM()
enum class EItemIndex : uint8
{
	Equip UMETA(DisplayName = "Equip Item"),
	Consume UMETA(DisplayName = "Consume Item"),
	Other UMETA(DisplayName = "Other Item"),
};


USTRUCT(BlueprintType,Blueprintable)
struct FItemStructStat: public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 item_ATK = 0;
	UPROPERTY(EditAnywhere)
	int32 Item_CRIT = 0;
	UPROPERTY(EditAnywhere)
	int32 item_CRITDMG = 0;

	UPROPERTY(EditAnywhere)
	int32 item_DEF = 0;
	UPROPERTY(EditAnywhere)
	int32 item_RES = 0;
	UPROPERTY(EditAnywhere)
	int32 item_SHI = 0;
};

USTRUCT(BlueprintType,Blueprintable)
struct FItemStruct: public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	int32 ItemTop = -1;
	UPROPERTY(EditAnywhere)
	int32 ItemIndex = 0;
	UPROPERTY(EditAnywhere)
	int32 ItemIndexType = 0;
	UPROPERTY(EditAnywhere)
	int32 ItemIndexDetail = 0;
	UPROPERTY(EditAnywhere)
	FString Name = FString("Name");
	UPROPERTY(EditAnywhere)
	FString NameType = FString("Type");
	UPROPERTY(EditAnywhere)
	FString NameDetail = FString("Detail");
	UPROPERTY(EditAnywhere)
	int32 ItemNum = 0;

	UPROPERTY(EditAnywhere)
    FItemStructStat ItemStructStat;
};

USTRUCT(BlueprintType,Blueprintable)
struct FItemMeshStruct
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere) // 1번
	TArray<UStaticMesh*>StaticMeshes;
	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*>Textures;
};

USTRUCT(BlueprintType,Blueprintable)
struct FItemStructType
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<FItemMeshStruct>ItemMeshTypes;
};

USTRUCT(BlueprintType,Blueprintable)
struct FItemStructIndex
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<FItemStructType>ItemMeshIndexes;
};

USTRUCT(BlueprintType,Blueprintable)
struct FItemStructTop
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<FItemStructIndex>ItemMeshTops;
};



