// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPC.generated.h"

UCLASS()
class PLAI_API ANPC : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//반경 확인
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Detection")
	class USphereComponent* PlayerDetectRange;

	UFUNCTION()
	void OnPlayerEnterRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPlayerExitRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	auto IsPlayerInRange() const -> bool; //거리판단

	UPROPERTY()
	bool bPlayerInRange = false; // 범위 안에 있는지 여부
	UPROPERTY()
	bool bNpcTrun=true;
	
	//NPC 인터페이스
	virtual void Interact(ACharacter* Interactor);

	void InteractTurnBased(ACharacter* Interactor);


};

