// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "ItemStruct.generated.h"



USTRUCT(BlueprintType,Blueprintable)
struct FItemStruct
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	FString Name = FString("Name");
	UPROPERTY(EditAnywhere)
	int32 ItemIndex = 0;
};

USTRUCT(BlueprintType,Blueprintable)
struct FItemClientStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<UStaticMesh*>StaticMeshes;
};