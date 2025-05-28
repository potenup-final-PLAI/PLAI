// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "PLAIPlayerController.generated.h"

class ATestPlayer;
/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class APLAIPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APLAIPlayerController();
	
	
	
	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationClickAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationTouchAction;

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	virtual void SetupInputComponent() override;
	
	// To add mapping context
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;
	
	/** Input handlers for SetDestination action. */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnTouchTriggered();
	void OnTouchReleased();

public:
	UFUNCTION(Server, Reliable)
	void Server_WarpPlayer(EMonSpawnType SpawnType);

	UPROPERTY(EditAnywhere,Replicated)
	TArray<ATestPlayer*>TestPlayers;
	
	UPROPERTY(EditAnywhere)
	float MiniMapTime = 0.0f;


//outline
	UPROPERTY()
	AActor* PreviousOutlineActor;

	void HandleMouseOutline();

	UPROPERTY(EditAnywhere)
	UMaterialInterface* OutlineMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* BaseMaterial;

	UPROPERTY()
	AActor* OutlineActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Outline")
	UMaterialInterface* OverlayOutlineMaterial;

	
private:
	FVector CachedDestination;

	bool bIsTouch; // Is it a touch device
	float FollowTime; // For how long it has been pressed

};


