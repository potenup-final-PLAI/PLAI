// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BaseBattlePawn.generated.h"

UCLASS()
class PLAI_API ABaseBattlePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseBattlePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(
		class UInputComponent* PlayerInputComponent) override;

	//----------Speed State-------------
	int32 speed = 0;
	//------------Turn System-----------------
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ATurnManager> turnManagerFactory;

	void OnTurnStart();
	void OnTurnEnd(ABaseBattlePawn* unit);
};
