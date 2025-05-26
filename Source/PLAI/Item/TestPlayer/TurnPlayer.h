// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TestPlayer.h"
#include "TurnPlayer.generated.h"

UCLASS()
class PLAI_API ATurnPlayer : public ATestPlayer
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATurnPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
