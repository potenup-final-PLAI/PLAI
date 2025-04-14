// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PLAI/Item/Item/ItemStruct.h"
#include "PLAI/Item/UI/Inventory/MenuInven.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemInven.h"
#include "InvenComp.generated.h"

class ATestPlayer;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAI_API UInvenComp : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInvenComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMenuInven> MenuInvenFactory;
	
	UPROPERTY(EditAnywhere)
	class UMenuInven* MenuInven;
public:
    UPROPERTY(EditAnywhere)
	ATestPlayer* TestPlayer;

	UPROPERTY(EditAnywhere)
	FItemStruct ItemStruct;

	void ItemInvenTory();

public:
	UPROPERTY(EditAnywhere)
	bool Flipflop = false;
};
