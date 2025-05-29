// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TurnManager.h"
#include "PhaseManager.generated.h"

UENUM()
enum class EBattlePhase : uint8
{
	None UMETA(DisplayName = "None"),
	BattleStart UMETA(DisplayName = "Battle Start"),
	RoundStart UMETA(DisplayName = "Round Start"), // 새로운 사이클 시작
	TurnProcessing UMETA(DisplayName = "Turn Processing"), // 유닛들 턴 진행 중
	RoundEnd UMETA(DisplayName = "Round End"),
	BattleEnd UMETA(DisplayName = "Battle End")
};

/**
 * 
 */
UCLASS()
class PLAI_API AUPhaseManager : public AGameStateBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// 구현 해야하는 내용
	// 상태에 따라 UI, 사운드, 이펙트 바꾸기
	// 전체 전투 흐름 관리
	//--------------------Data Init--------------------
	void InitBattle();
	//--------------------Cycle System--------------------
	UPROPERTY(ReplicatedUsing = OnRep_UpdateCycleUI, VisibleAnywhere,
		BlueprintReadOnly, Category = "Cycle")
	int32 cycle = 0;

	void SetCycle();
	void UpdateCycleUI();
	UFUNCTION()
	void OnRep_UpdateCycleUI();
	//--------------------Phase System--------------------
	// 멀티로 변경 할 때 ReplicatedUsing을 활용하여 함수 호출 하자
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPhase, VisibleDefaultsOnly,
		BlueprintReadOnly, Category = "Phase")
	EBattlePhase currentPhase;

	// Phase 관리(서버에서만 실행)
	void SetPhase(EBattlePhase phase);
	UFUNCTION()
	void OnRep_CurrentPhase();
	//--------------------Turn System--------------------
	// currentActionPoints = maxActionPoints;
	// UI 안내용	"플레이어 턴입니다" UI 출력
	// 페이즈 한정 버프/디버프	플레이어 페이즈 시작 시 체력 회복 등
	// 전체 행동 제한	모든 유닛이 행동 못하게 하고 Cutscene 등 삽입
	// AI 판단	적 페이즈에만 그룹 AI 로직 실행

	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadOnly,
		Category = "Phase")
	ATurnManager* turnManager;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Phase")
	TSubclassOf<ATurnManager> turnManagerFactory;

	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadOnly,
		Category = "Phase")
	TArray<ABaseBattlePawn*> unitQueue;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Phase")
	TArray<ABaseBattlePawn*> httpUnitQueue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phase")
	TSubclassOf<ABaseBattlePawn> unitFactory;

	// 유닛 첫 큐 세팅
	void SetUnitQueue();
	// 유닛 Array 큐 처럼 사용하기 위한 업데이트
	void SortUnitQueue();
	// 턴 끝날 때 유닛 포함 안시키게
	void SortUnitTurnEnd();
	// 이미 죽은 유닛 갱신하는 함수
	ABaseBattlePawn* PopNextAliveUnit();
	// 전투 시작
	void StartBattle();
	// Unit이 Player인지 Enemy인지 판별
	void TurnPorcessing(ABaseBattlePawn* unit);
	// 플레이어 턴 시작
	void StartPlayerPhase();
	// 플레이어 턴 종료
	UFUNCTION(Server, Reliable)
	void ServerRPC_EndPlayerPhase();
	// 적 턴 시작
	//void StartEnemyPhase();
	// 적 턴 종료
	UFUNCTION(Server, Reliable)
	void ServerRPC_EndEnemyPhase();
	// 전투 종료
	void BattleEnd();

	//--------------Start Battle API ------------------
	FTimerHandle timerBattleStartHandle;

	// 게임 시작 전 세팅 작업
	void SetBeforeBattle();
	// StartBattle API 호출
	void TrySendInitialState();
	// StartBattle API 들어갈 데이터 세팅 함수
	FEnvironmentState SetStartBattleAPI();

	//------------------Battle Action API-----------------------
	// HttpActor 탐색용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	TSubclassOf<class ABattleHttpActor> httpActorFactory;

	// BattleAction API 호출
	void TrySendbattleState(ABaseBattlePawn* unit);
	// BattleAction API에 들어갈 데이터 세팅 함수
	FBattleTurnState SetBattleProcessingAPI();
	// unit에 state가 세팅 되었는지 체크 하는 함수
	bool AreAllUnitsInitialized() const;

	//----------------GridTileManager Tile 관리를 위한 선언--------------------------- 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Phase")
	class AGridTileManager* gridTileManager;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	TSubclassOf<class AGridTileManager> girdTileManagerFactory;

	//-------------Set Status-----------------------
	bool bIsInitialized = false;

	void TryInitStatus(ABaseBattlePawn* unit);
	void SetStatus(ABaseBattlePawn* unit);
	//------------Status 이름 변경--------------------
	FString GetStatusEffectsString(EStatusEffect effect);

	//-----------Unit UI 이름 확인 쉽게 세팅할 변수------------
	int8 unitPlayerNameindex = 0;
	int8 unitEnemyNameindex = 0;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phase")
	class UWorldGi* gi;

	//-------------Damage Actor Widget-----------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DamageUI")
	class AWorldDamageUIActor* damageUIActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageUI")
	TSubclassOf<class AWorldDamageUIActor> damageUIFactory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Phase")
	class APlayerController* pc;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Phase")
	class ABattleHUD* hud;
	
	void InitOtherClass();

	// TurnOrder에 사용할 Array
	UPROPERTY(Replicated, EditAnywhere)
	TArray<class ABaseBattlePawn*> turnOrderArray;
	
	//----------------Player Ready Check------------------
	UPROPERTY(EditAnywhere)
	int32 readyCount = 0;

	void PlayerReady(APlayerController* playerControl);

	
	void DoingPopNextAliveUnit();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PopNextAliveUnit(ABaseBattlePawn* nextCurUnit);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString NetLog;

	void ClearNetLog();
	void AddNetLog(FString msg);
	
};
