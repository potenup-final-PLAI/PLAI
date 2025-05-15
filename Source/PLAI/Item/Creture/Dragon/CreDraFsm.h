// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "PLAI/Item/Creture/CreFsm.h"
#include "PLAI/Item/Creture/CreBullet.h"
#include "CreDraFsm.generated.h"

UENUM(BlueprintType)
enum class EDraState : uint8
{
	DraIdle UMETA(DisplayName = "Idle"),
	
	DraAround UMETA(DisplayName = "Around"),

	DraAttackRangePre UMETA(DisplayName = "DraAttackRangePre"),
	DraAttackRange UMETA(DisplayName = "DraAttackRange"),
	DraAttackRangeFinish UMETA(DisplayName = "DraAttackRangeFinish"),
	
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
	float DistTarget = 0.0f;
	
	UPROPERTY(EditAnywhere)
	EDraState Drastate = EDraState::DraIdle;
	
    UPROPERTY(EditAnywhere)
	TArray<UNiagaraSystem*>NiagaraSkills;

	UPROPERTY(EditAnywhere)
	TArray<UParticleSystem*>ParticlesSkills;
	
	UPROPERTY(EditAnywhere)
	TArray<AMonster*>Monsters;
	
	void DraIdle(float time = 1.0f);
	
	void DraAround(float time = 1.0f);

	void DraAttackRangePre(float time = 1.0f);
	void DraAttackRange(float time = 1.0f);
	void DraAttackRangeFinish(float time = 1.0f);

	void DraAttackSingleRange(float Radios = 2000.0f, float time = 2.0f);
	
	void DraAttackMultiPre(float time = 1.0f, float Radius = 2000.0f);
	void DraAttackMulti(float time = 1.0f);
	
    UPROPERTY(EditAnywhere)
	TArray<float>TimerMultiPre;
	
	UPROPERTY(EditAnywhere)
	float TimerMulti = 0.0f;
	UPROPERTY(EditAnywhere)
	int32 FinishCount = 0;
	
	void MyTimer(TFunction<void()> func, float time = 2.0f);

	void NextState();

	UPROPERTY(EditAnywhere)
	class ACreDragon* Dragon;

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

	float CurrentTime = 0;

	FVector AttackVector;
	FVector FireBallStart = FVector::ZeroVector;
	FVector FireBallEnd = FVector::ZeroVector;

};
