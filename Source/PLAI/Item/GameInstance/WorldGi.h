// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WorldGi.generated.h"

/**
 * 
 */
UCLASS(Blueprintable,Blueprintable)
class PLAI_API UWorldGi : public UGameInstance
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere)
	TArray<int32>bWorldSpawnInt;
	
	UPROPERTY(EditAnywhere)
	int32 bWorldSpawnTime = 0;
};
