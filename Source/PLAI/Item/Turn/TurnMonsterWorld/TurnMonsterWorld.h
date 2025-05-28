// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PLAI/Item/Monster/MonsterStruct.h"
#include "TurnMonsterWorld.generated.h"

UCLASS()
class PLAI_API ATurnMonsterWorld : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATurnMonsterWorld();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereComp;
	
	UPROPERTY(EditAnywhere)
	FVector TargetLocation;
	float CurrentTime = 0;
	void RandomLocation();

	FVector RandomLoc(float x = 200, float y = 200);
};
