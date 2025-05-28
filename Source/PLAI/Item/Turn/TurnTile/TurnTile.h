// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurnTile.generated.h"

class ATestPlayer;

UCLASS()
class PLAI_API ATurnTile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATurnTile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComp;

	float CurrentTime = 0;

	UPROPERTY(EditAnywhere)
	UDataTable* MonsterTable;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ATurnPlayer> TurnPlayerFactory;

	TArray<FVector>GetTileCorner();

	void MonsterSpawn();
    void MonsterSpawnTable(FVector SpawnLocation);

	void PlayerSpawn();
	// void PlayerSpawnFactory(FVector SpawnLocation);
};
