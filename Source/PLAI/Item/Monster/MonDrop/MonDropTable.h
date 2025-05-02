// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MonDropTable.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType,Blueprintable)
struct FMonDropTable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop")
	FDataTableRowHandle ItemRowHandle;
	// ② 드롭 확률 (0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop")
	float DropChance = 1.0f;
};
