// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// DECLARE_DELEGATE_OneParam(FOnWalp,class ATestPlayer* testplayer);

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameFramework/Actor.h"
#include "PLAI/Item/Monster/MonSpawn/MonSpawn.h"
#include "Warp.generated.h"

class ATestPlayer;

UCLASS()
class PLAI_API AWarp : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWarp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// FOnWalp OnWalp;

	UPROPERTY(EditAnywhere)
	EMonSpawnType MonSpawnType;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUiPortal>UiPortalFactory;

	UPROPERTY(EditAnywhere)
	class UUiPortal* UiPortal;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComp;
	
	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* NiagaraComp;
	
	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereComp;

	UPROPERTY(EditAnywhere)
	TArray<FString>LevelPath;
	UPROPERTY(EditAnywhere)
	TArray<FString>OldLevelPath;
	UPROPERTY(EditAnywhere)
	TArray<FString>NewLevelPath;
	
    UPROPERTY(EditAnywhere)
	TArray<bool>bWarpLevel;
	
	UPROPERTY(EditAnywhere)
	TArray<FVector>WarpLocation;
	
	UFUNCTION()
	void OnOverlappedWarp(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UFUNCTION()
	void OnEndOvelappedWarp(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(Server, Reliable)
	void Server_WarpPlayer(ATestPlayer* TestPlayer);

	// UFUNCTION()
	// void HandleOldLevelLoaded();

	void WarpLevel(class ATestPlayer* TestPlayer, int32 index);
};
