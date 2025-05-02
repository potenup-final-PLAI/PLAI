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
	
	DraAttackSingleRange UMETA(DisplayName = "DraAttackSingleRange"),
	
	DraAttackMultiPre UMETA(DisplayName = "DraAttackMultiPre"),
	DraAttackMulti UMETA(DisplayName = "DraAttackMulti"),
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
	EDraState Drastate = EDraState::DraAttackMultiPre;

	UPROPERTY(EditAnywhere)
	TArray<AMonster*>Monsters;
	
	void DraIdle(float time = 1.0f);
	
	void DraAround(float time = 1.0f);
	void DraPatrol(float time = 1.0f);

	void DraAttackSingleRange(float Radios = 2000.0f, float time = 2.0f);
	
	void DraAttackMultiPre(float time = 1.0f, float Radius = 1000.0f);
	void DraAttackMulti(float time = 1.0f);
	
	UPROPERTY(EditAnywhere)
	int32 MultiCount = 0;
	UPROPERTY(EditAnywhere)
	int32 FinishCount = 0;
	
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
