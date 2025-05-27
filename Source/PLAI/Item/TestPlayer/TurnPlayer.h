// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TestPlayer.h"
#include "PLAI/Item/Turn/TurnMoster/TurnMonster.h"
#include "TurnPlayer.generated.h"

UENUM(BlueprintType)
enum class ETurnPlayerState : uint8
{
	Idle,
	Move,
	Attack,
	Avoid
};

USTRUCT(BlueprintType)
struct FTurnPlayerStruct
{
	GENERATED_BODY()
public:
	int32 CurrentHp = 100;
	int32 MaxHp = 100;
	int32 Atk = 50;
};
UCLASS()
class PLAI_API ATurnPlayer : public ACharacter
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

public:
	UPROPERTY(EditAnywhere)
	ETurnPlayerState TurnPlayerState = ETurnPlayerState::Move;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUITurnHpBar> TurnHpBarFactory;
	UPROPERTY(EditAnywhere)
	class UUITurnHpBar* UITurnHpBar;

	UPROPERTY(EditAnywhere)
	class AGameStateOpen* GameState;

	UPROPERTY(EditAnywhere)
	class AAIController* AI;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* WidgetComp;
	
	UPROPERTY(EditAnywhere)
	FVector MoveLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	bool bTurn = false;

	UPROPERTY(EditAnywhere)
	int32 TurnIndex = 0;
	
	UPROPERTY(EditAnywhere)
	FVector2D TileRange2D;

	UPROPERTY(EditAnywhere)
	FTurnPlayerStruct TurnPlayerStruct;

	UPROPERTY(EditAnywhere)
	FTimerHandle TurnTimerHandle;

	void PlayerState(FVector Location, ATurnMonster* TurnMonster);

	void Idle();
	void MoveToMonster(FVector Location, ATurnMonster* TurnMonster);
	void AttackToMonster(class ATurnMonster* TurnMonster);
	void AvoidToMonster();
	
	UFUNCTION()
	void OnAIMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	
	// void ChangeState(ETurnPlayerState NewState);
};
