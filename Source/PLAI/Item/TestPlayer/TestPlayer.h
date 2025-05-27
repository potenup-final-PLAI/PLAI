// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PLAI/Item/UI/Main/UIChaMain.h"
#include "PLAI/Public//BaseFile/PLAICharacter.h"
#include "TestPlayer.generated.h"

UCLASS()
class PLAI_API ATestPlayer : public APLAICharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATestPlayer();

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

	UPROPERTY(EditAnywhere)
	class ULogItemComp* LogItemComp;

	UPROPERTY(EditAnywhere)
	class UInputComp* InputComp;
	
	UPROPERTY(EditAnywhere)
	class UCreComp* CreComp;

	UPROPERTY(EditAnywhere)
	class UTurnComp* TurnComp;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ATestPlayer> TestPlayerFactory;

	UPROPERTY(EditAnywhere)
	class ATestPlayer* TestPlayerParent;
	
	UPROPERTY(EditAnywhere)
	class USceneCaptureComponent2D* CaptureComp;

	bool bCameraChane = false;
};
