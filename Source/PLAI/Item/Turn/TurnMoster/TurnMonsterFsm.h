// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TurnMonsterFsm.generated.h"

class ATurnPlayer;
// DECLARE_DELEGATE_OneParam(FOnAttackPlayer, ATurnPlayer)

struct FAIRequestID;

namespace EPathFollowingResult
{
	enum Type : int;
}

UENUM(BlueprintType)
enum class ETurnMonsterState : uint8
{
	Idle,
	Move,
	Attack
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAI_API UTurnMonsterFsm : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTurnMonsterFsm();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
	ETurnMonsterState TurnMonsterState = ETurnMonsterState::Idle;

	void IdleState();
	void MoveState();
	void AttackState();
	
	void MoveToPlayer();

	UFUNCTION()
	void AttackToPlayer(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	
	UPROPERTY(EditAnywhere)
	class ATurnMonster* TurnMonster;

	UPROPERTY(EditAnywhere)
	class AGameStateOpen* GameState;

	UPROPERTY(EditAnywhere)
	class ATurnPlayer* TurnPlayer;

};
