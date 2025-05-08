// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MonFsm.generated.h"

class AMonster;

UENUM(BlueprintType,Blueprintable)
enum class EMonState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Around UMETA(DisplayName = "Arround"),
	Attack UMETA(DisplayName = "Attack"),
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAI_API UMonFsm : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMonFsm();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
	class AMonster* Monster;

    UPROPERTY(EditAnywhere)
	float CurrentTime = 0.0f;
	
	UPROPERTY(EditAnywhere)
	EMonState MonState = EMonState::Idle;

	UPROPERTY(EditAnywhere)
	FVector InitLocation;
	UPROPERTY(EditAnywhere)
	float InitLocationFloat = 3000.0f;
	
	UPROPERTY(EditAnywhere)
	FVector TargetLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere)
	FVector RandLoc;

	UPROPERTY(EditAnywhere)
	float distLength;

	void Idle();
	void Around();
	void Attack();

	bool bMoveArrive = false;

	FVector RandLocation(float X = 1000.0f, float Y = 1000.0f, float Z = 50.0f);
	float RandLocationFloat = 1000.0f;

	void MoveDestination();
	void LineDestination();

	bool bTimer = false;
	bool bRotator =false;

	void MyTimer(TFunction<void()> Func, float time = 2.0f);
};
