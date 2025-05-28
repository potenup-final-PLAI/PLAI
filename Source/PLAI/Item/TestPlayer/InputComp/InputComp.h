// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

DECLARE_DELEGATE(FInputMap);

#include "CoreMinimal.h"
#include "BaseFile/PLAIPlayerController.h"
#include "Components/ActorComponent.h"
#include "InputComp.generated.h"


struct FInputActionValue;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAI_API UInputComp : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInputComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	FInputMap OnInputMap;

	UPROPERTY(EditAnywhere)
	class ATestPlayer* TestPlayer;
	UPROPERTY(EditAnywhere)
	class ATurnPlayer* TurnPlayer;
	UPROPERTY(EditAnywhere)
	class APlayerController* Pc;
	
	UPROPERTY(EditAnywhere)
	float WalkTime= 0;
	
	UPROPERTY(EditAnywhere)
	float TimeCamera = 0;
	UPROPERTY(EditAnywhere)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere)
	UInputAction* IE_Equip;
	UFUNCTION()
	void On_Equip();

	UPROPERTY(EditAnywhere)
	UInputAction* IE_Inven;
	UFUNCTION()
	void On_Inven();

	UPROPERTY(EditAnywhere)
	UInputAction* IE_Stat;
	UFUNCTION()
	void On_Stat();
	
	UPROPERTY(EditAnywhere)
	UInputAction* IE_Jump;
	UFUNCTION(Server, Reliable)
	void Server_On_Jump();
	UFUNCTION()
	void On_Jump();

	UPROPERTY(EditAnywhere)
	UInputAction* IE_Map;
	UFUNCTION()
	void On_Map();

	UPROPERTY(EditAnywhere)
	bool bLeftMouse = false;
	UPROPERTY(EditAnywhere)
	float MousePowerX = 0;
	UPROPERTY(EditAnywhere)
	float MousePowerY = 0;
	UPROPERTY(EditAnywhere)
	float MouseTime = 0;
	
	UPROPERTY(EditAnywhere)
	UInputAction* IE_LeftMouse;
	
	UFUNCTION()
	void On_LeftMouseStart();
	UFUNCTION()
	void On_LeftMouseTriggered();
	UFUNCTION()
	void On_LeftMouseComplete();
	
	UPROPERTY(EditAnywhere)
	UInputAction* IE_MouseWheel;
	UFUNCTION()
	void On_MouseWheelTriggered(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere)
	UInputAction* IE_RotateView;
	UPROPERTY(EditAnywhere)
	bool bRotateView = false;
	UFUNCTION()
	void On_RoatateView();

	virtual void InitializeComponent() override;
	
	void BindInputActions();
	void SetMappingContext();
};
