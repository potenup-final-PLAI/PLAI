// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PLAI/Item/Monster/MonFsm.h"
#include "MonWorldFsm.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAI_API UMonWorldFsm : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMonWorldFsm();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
