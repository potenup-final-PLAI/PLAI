// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseBattlePawn.h"
#include "GameFramework/Actor.h"
#include "TurnManager.generated.h"

UENUM()
enum class ETurnState : uint8
{
	None UMETA(DisplayName = "None"),
	PlayerTurn UMETA(DisplayName = "PlayerTurn"),
	EnemyTurn UMETA(DisplayName = "EnemyTurn"),
	TurnEnd UMETA(DisplayName = "TurnEnd")
};

UCLASS()
class PLAI_API ATurnManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATurnManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//--------------Turn State---------------------
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Turn")
	ETurnState curTurnState;

	void SetTurnState(ETurnState newTurnState);
	//--------------Turn Start---------------------
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Turn")
	class AUPhaseManager* phaseManager;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Turn")
	ABaseBattlePawn* curEnemy;
	// 유닛들의 턴 순서를 저장하는 배열
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Turn")
	TArray<ABaseBattlePawn*> enemyQueue;

	// 적 턴 List 세팅
	void SetEnemyTurnList(TArray<ABaseBattlePawn*>& enemies);
	// 적 첫 유닛 실행
	void StartEnemyTurn();
	// 다음 적 유닛 실행
	void StartNextEnemyTurn();
};
