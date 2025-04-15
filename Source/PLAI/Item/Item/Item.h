// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemStruct.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

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

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemStruct ItemStruct;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItemMeshStructIndex> ItemMeshStructIndexArray;
	
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMesh;
	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxComp;

	virtual void SetMesh();
	int32 RandIndex();
};
