// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/Util/BattleType/BattleTypes.h"
#include "GameFramework/Actor.h"
#include "BattleHttpActor.generated.h"

UCLASS()
class PLAI_API ABattleHttpActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABattleHttpActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FBattleTurnState battleState;
	FActionRequest ActionRequestData;
	
	void HttpPost(FEnvironmentState environmentState = FEnvironmentState(), FBattleTurnState battleTurnState = FBattleTurnState(), class ABaseBattlePawn* unit = nullptr);
	bool IsEmptyEnvironmentState(const FEnvironmentState& state);
	bool IsEmptyBattleTurnState(const FBattleTurnState& state);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateDebugLog(const FString& log);
};
