// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonSpawn.generated.h"

UENUM(BlueprintType)
enum class EMonSpawnType : uint8
{
	 Village UMETA(DisplayName = "Village"),
	Mountain UMETA(DisplayName = "Mountain"),
	  Desert UMETA(DisplayName = "Desert"),
	 Dungeon UMETA(DisplayName = "Dungeon"),
};

UCLASS()
class PLAI_API AMonSpawn : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMonSpawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class AMonster>> MonsterFactory;
	
	UPROPERTY(EditAnywhere)
	TArray<TWeakObjectPtr<AMonster>> Monsters;
	
	// UPROPERTY(EditAnywhere)
	// TArray<class AMonster*> Monsters;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComp;

    UPROPERTY(EditAnywhere)
	EMonSpawnType MonSpawnType;
	
	UPROPERTY(EditAnywhere)
	bool bTimer = false;
	
	UPROPERTY(EditAnywhere)
	float CurrentTime = 0;

	UPROPERTY(EditAnywhere)
	FVector RandLoc;
	UPROPERTY(EditAnywhere)
	FVector HitLoc;

	UPROPERTY(EditAnywhere)
	UDataTable* MonsterTable;

	void MyTimer(void(AMonSpawn::*Func)(),float Second = 2.0f);
	void MyTimer(TFunction<void()> Func, float Second = 2.0f);
	
	FVector RandLocation(float X = 1000.0f, float Y = 1000.0f, float Z = 50.0f);
	void SpawnMonster();

	// UPROPERTY(EditAnywhere)
	// TSubclassOf<UUIChaMain>UUIChaMainFactory;
	//
	// UPROPERTY(EditAnywhere)
	// class UUIChaMain* UIChaMain;
};
