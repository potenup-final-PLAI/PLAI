// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UiMonWorld.h"
#include "PLAI/Item/Monster/MonsterMaster.h"
#include "MonWorld.generated.h"

UENUM(BlueprintType)
enum class EMonsterType : uint8
{
	Monster UMETA(DisplayName = "Monster"),
	Boss UMETA(DisplayName = "Boss"),
};

UCLASS()
class PLAI_API AMonWorld : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMonWorld();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	EMonsterType MonsterType;
	
    UPROPERTY()
	float CurrentTime = 0;

	UPROPERTY()
	float CurrentTimeTwo = 0;
	
	UPROPERTY(EditAnywhere)
	float CastingDistance = 500.0f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUiMonWorld> UIMonWorldFactory;
	UPROPERTY(EditAnywhere)
	class UUiMonWorld* UIMonWorld;
	
	UPROPERTY(EditAnywhere)
	FVector FirstInitLoc;
	UPROPERTY(EditAnywhere)
	FVector InitLoc = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	float RandLocationDist = 350.0f;

	UPROPERTY(EditAnywhere)
	float InitLocationDist = 800.0f;

	UPROPERTY(EditAnywhere)
	bool bBattle = false;
	
	FVector RandLocation();
	
	void MoveToLocation();

	TArray<FOverlapResult> GetHitResult(float Distance = 50.0f);
	
	void CastPlayer();

	void CastObject();
};
