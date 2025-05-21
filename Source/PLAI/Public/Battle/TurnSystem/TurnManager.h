// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "BaseBattlePawn.h"
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
	virtual void GetLifetimeReplicatedProps(
		TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//--------------Turn State---------------------
	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadOnly,
		Category = "Turn")
	ETurnState curTurnState;

	void SetTurnState(ETurnState newTurnState);
	//--------------Turn Start---------------------
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Turn")
	class AUPhaseManager* phaseManager;

	UPROPERTY(ReplicatedUsing = OnRep_UpdateWhoTurn, VisibleDefaultsOnly,
		BlueprintReadOnly,
		Category = "Turn")
	ABaseBattlePawn* curUnit;
	//------------누구 턴인지 나타내는 UI --------

	void UpdateWhoTurn();
	UFUNCTION()
	void OnRep_UpdateWhoTurn();

	// 유닛들의 턴 순서를 저장하는 배열
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Turn")
	TArray<ABaseBattlePawn*> enemyQueue;

	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadOnly,
		Category = "Turn")
	int32 turnCount = 0;
	//--------------Player Turn---------------------
	// 플레이어 첫 유닛 실행
	void StartPlayerTurn();


	//--------------Enemy Turn---------------------
	// 적 첫 유닛 실행
	void StartEnemyTurn();


	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_CameraChange(class APawn* target);
};
