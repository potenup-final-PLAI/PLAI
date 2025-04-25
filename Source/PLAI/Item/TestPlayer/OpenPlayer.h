// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TestPlayer.h"
#include "KHA/KHACharacter.h"
#include "OpenPlayer.generated.h"

UCLASS()
class PLAI_API AOpenPlayer : public AKHACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AOpenPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(EditAnywhere)
	class UInvenComp* InvenComp;

	UPROPERTY(EditAnywhere)
	class UStoreComp* StoreComp;

	UPROPERTY(EditAnywhere)
	class ULoginComp* LoginComp;
};
