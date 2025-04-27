// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridTile.generated.h"

UCLASS()
class PLAI_API AGridTile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGridTile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* boxComp;

	UPROPERTY(VisibleAnywhere)
	FIntPoint gridCoord;

	void SetGridCoord(FIntPoint coord);

	//--------------------Move System------------------------
	float sCostValue = 0;
	float tCostValue = 0;
	AGridTile* parentTile;

	UPROPERTY(EditAnywhere)
	class UImage* outLine;
	
	void SetCost(AGridTile* s, AGridTile* g);
	void SetColor(FLinearColor color);
};
