// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseBattlePawn.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "TurnManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(TPS, Log, All);

#define CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define PRINT_CALLINFO() \
UE_LOG(TPS, Warning, TEXT("%s"), *CALLINFO)

#define PRINTLOG(fmt, ...) \
UE_LOG(TPS, Warning, TEXT("%s %s"), *CALLINFO, *FString::Printf(fmt, ##__VA_ARGS__))

#define PRINTLOGTOSCREEN(fmt, ...) \
PRINTLOG(fmt, ##__VA_ARGS__); \
if (GEngine) \
{ \
GEngine->AddOnScreenDebugMessage(777, 0.f, FColor::Cyan, *FString::Printf(fmt, ##__VA_ARGS__)); \
}

inline void PrintLogFunc(const TCHAR* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	TCHAR buffer[1024];
	FCString::GetVarArgs(buffer, UE_ARRAY_COUNT(buffer), fmt, args);
	PRINTLOG(TEXT("%s"), buffer);

	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Cyan,
	                                 *FString::Printf(TEXT("%s"), buffer));

	va_end(args);
}

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
	ABaseBattlePawn* curUnit;
	// 유닛들의 턴 순서를 저장하는 배열
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Turn")
	TArray<ABaseBattlePawn*> enemyQueue;

	//--------------Player Turn---------------------
	// 플레이어 첫 유닛 실행
	void StartPlayerTurn();
	// 플레이어 다음 유닛 실행
	void StartNextPlayerTurn();

	//--------------Enemy Turn---------------------
	// 적 첫 유닛 실행
	void StartEnemyTurn();
	// 다음 적 유닛 실행
	void StartNextEnemyTurn();
	
};
