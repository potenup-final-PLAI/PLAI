// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PLAI/Item/Creture/CreFsm.h"
#include "CreDraFsm.generated.h"

UENUM(BlueprintType)
enum class EDraState : uint8
{
	Weapon UMETA(DisplayName = "Weapon"),
	Idle UMETA(DisplayName = "Idle"),
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAI_API UCreDraFsm : public UCreFsm
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCreDraFsm();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
};
