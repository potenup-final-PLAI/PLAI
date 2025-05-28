// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "PLAI/Item/Monster/Monster.h"
#include "PLAI/Item/Monster/MonsterStruct.h"
#include "PLAI/Item/Turn/TurnMonsterWorld/TurnMonsterWorld.h"
#include "TurnMonster.generated.h"

UCLASS()
class PLAI_API ATurnMonster : public AMonster
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATurnMonster();

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
	class UTurnMonsterFsm* TurnMonsterFsm;
	
	UPROPERTY(EditAnywhere)
	FTurnMonsterStruct TurnMonsterStruct;

	UPROPERTY(EditAnywhere)
	int32 TurnIndex = 0;

	UPROPERTY(EditAnywhere)
	bool bTurn = false;

	UPROPERTY(EditAnywhere)
	AAIController* AiController;

	UPROPERTY(EditAnywhere)
	class AGameStateOpen* GameState;

	UPROPERTY(EditAnywhere)
	FVector2D TileRange2D;

	virtual void SetMonsterUi() override;

	virtual void SetHpBar() override;
};
