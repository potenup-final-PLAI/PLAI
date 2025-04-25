// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPC2.generated.h"

UCLASS()
class ANPC2 : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void NotifyActorOnClicked(FKey ButtonPressed);
	
	int bZoomedIn;

	//범위 인식
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* DetectionSphere;

	UFUNCTION()
	void OnPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
					   const FHitResult& SweepResult);
	UFUNCTION()
	void OnPlayerExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
