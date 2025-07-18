﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndingPortal.generated.h"

UCLASS()
class PLAI_API AEndingPortal : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEndingPortal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//-------------------기본 콜리전 세팅----------------------
	UPROPERTY(EditAnywhere)
	class UBoxComponent* boxComp;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* meshComp;

	//------------Ending---------------
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UEndingCredit> endingCreditFactory;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EndingUI();
};
