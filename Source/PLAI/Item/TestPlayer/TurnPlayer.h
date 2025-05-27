// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TestPlayer.h"
#include "TurnPlayer.generated.h"
USTRUCT(BlueprintType)
struct FTurnPlayerStruct
{
	GENERATED_BODY()
public:
	int32 CurrentHp = 100;
	int32 MaxHp = 100;
	int32 Atk = 10;
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
	TSubclassOf<class UUITurnHpBar> TurnHpBarFactory;
	UPROPERTY(EditAnywhere)
	class UUITurnHpBar* UITurnHpBar;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* WidgetComp;
	
	UPROPERTY(EditAnywhere)
	FVector MoveLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	bool bTurn = false;

	UPROPERTY(EditAnywhere)
	int32 TurnIndex = 0;

	UPROPERTY(EditAnywhere)
	FTurnPlayerStruct TurnPlayerStruct;
	
	void MoveToPlayer();
};
