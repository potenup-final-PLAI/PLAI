// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldDamageUIActor.generated.h"

UCLASS()
class PLAI_API AWorldDamageUIActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWorldDamageUIActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* damageUIComp;
	UPROPERTY(EditAnywhere)
	class UWorldDamageUI* damageUI;
	
	float speed = 250;
	bool bShowUI = false;

	FVector startLoc;
	FVector endLoc;
	float moveDuration = 2.f;
	float elapsed = 0.f;
	
	FTimerHandle damageTimerHandle;
	void ShowDamageUI();
	void HideDamageUI();
};
