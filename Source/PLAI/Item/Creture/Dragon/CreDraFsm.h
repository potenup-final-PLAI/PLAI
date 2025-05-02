// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PLAI/Item/Creture/CreFsm.h"
#include "CreDraFsm.generated.h"
class AMonster;

UENUM(BlueprintType)
enum class EDraState : uint8
{
	DraIdle UMETA(DisplayName = "Idle"),
	DraAround UMETA(DisplayName = "Around"),
	DraPatrol UMETA(DisplayName = "Attack"),
	DraAttack UMETA(DisplayName = "Patrol"),
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAI_API UCreDraFsm : public UCreFsm
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCreDraFsm();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
public:
	UPROPERTY(EditAnywhere)
	EDraState Drastate = EDraState::DraIdle;

	UPROPERTY(EditAnywhere)
	TArray<AMonster*>Monsters;
	UPROPERTY(EditAnywhere)
	int32 AttackCount = 0;
	UPROPERTY(EditAnywhere)
	int32 AttackCounted = 0;
	
	void DraIdle();
	void DraAround();
	void DraPatrol();
	void DraAttack(float Radius = 1000.0f);
	void DraAttackSingleRange(float Radios = 2000.0f, float time = 2.0f);
	
	void MyTimer(void(UCreDraFsm::*Func)(), float time);
	void MyTimer(TFunction<void()> func, float time = 2.0f);

	void NextState();

	UPROPERTY(EditAnywhere)
	class ACreDragon* Dragon;
	
	UPROPERTY(EditAnywhere)
	class ATestPlayer* TestPlayer;

	UPROPERTY(EditAnywhere)
	float RotateTime = 0;
    UPROPERTY(EditAnywhere)
	float TimeAttack = 0;
	UPROPERTY(EditAnywhere)
	float TimeFire = 0;
	
	UPROPERTY(EditAnywhere)
	bool bTimer = true;

	UPROPERTY(EditAnywhere)
	TArray<FVector>PatrolPoints;
	
	UPROPERTY(EditAnywhere)
	TArray<FVector>AttackPoints;
	
	UPROPERTY(EditAnywhere)
	int32 PatrolIndex = 0;

	float CurrentTime = 0;

	FVector AttackVector;
	FVector FireBallStart = FVector::ZeroVector;
	FVector FireBallEnd = FVector::ZeroVector;

};
