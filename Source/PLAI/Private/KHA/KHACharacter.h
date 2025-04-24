// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PLAI/Public//BaseFile/PLAICharacter.h"
#include "KHACharacter.generated.h"

UCLASS()
class AKHACharacter : public APLAICharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AKHACharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void TryZoomOnClick();
	
	UFUNCTION()
	void TryInteract();

	USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	
	bool bZoomedIn = false;

	//목적지 관련
	UFUNCTION()
	void HandleClickLocation();
	FVector LastClickLocation;
	bool bShouldMove = false;

	//레벨전환
	void TryLevelChange();
};
