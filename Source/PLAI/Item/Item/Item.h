// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemStruct.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"
UENUM(BlueprintType)
enum class EItemIndex : uint8
{
	Equip UMETA(DisplayName = "Equip Item"),
	Consume UMETA(DisplayName = "Consume Item"),
	Other UMETA(DisplayName = "Other Item"),
};

UCLASS()
class PLAI_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	UPROPERTY(EditAnywhere)
	int8 Eindex;
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemStruct ItemStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemMeshStruct> ItemMeshStructs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemIndex EItemIndex;
	
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMesh;
	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxComp;

	virtual void SetMesh();
	int32 RandIndex();
};
