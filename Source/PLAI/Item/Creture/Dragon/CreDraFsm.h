// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PLAI/Item/Creture/CreFsm.h"
#include "CreDraFsm.generated.h"

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

	void DraIdle();
	void DraAround();
	void DraPatrol();
	void DraAttack();
	
	void MyTimer(void(UCreDraFsm::*Func)(), float time);
	void MyTimer(TFunction<void()> func, float time = 2.0f);

	void NextState();

	UPROPERTY(EditAnywhere)
	class ACreDragon* Dragon;
	
	UPROPERTY(EditAnywhere)
	class ATestPlayer* TestPlayer;

	bool bTimer = true;
	float RotateTime;

	UPROPERTY(EditAnywhere)
	TArray<FVector>PatrolPoints;
	
	UPROPERTY(EditAnywhere)
	int32 PatrolIndex = 0;

	float CurrentTime = 0;

	void OverlappedSphere();
};
