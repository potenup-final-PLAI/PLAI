// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PLAI/Item/Item/Item.h"
#include "ItemEquip.generated.h"

UCLASS()
class PLAI_API AItemEquip : public AItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItemEquip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	
};
