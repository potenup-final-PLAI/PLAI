// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TurnManager.h"
#include "GameFramework/GameStateBase.h"
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

public:
	// 구현 해야하는 내용
	// 상태에 따라 UI, 사운드, 이펙트 바꾸기
	// 전체 전투 흐름 관리
	//--------------------Data Init--------------------
	void InitBattle();
	//--------------------Cycle System--------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cycle")
	int32 cycle = 0;

	void SetCycle();
	//--------------------Phase System--------------------
	// 멀티로 변경 할 때 ReplicatedUsing을 활용하여 함수 호출 하자
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Phase")
	EBattlePhase currentPhase;

	// Phase 관리(서버에서만 실행)
	void SetPhase(EBattlePhase phase);
	//--------------------AP System--------------------
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Phase")
	int32 maxActionPoints = 5;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Phase")
	int32 curPlayerAP = 0;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Phase")
	int32 curEnemyAP = 0;

	// Player AP 세팅
	void SetPlayerAP();
	// Enemy AP 세팅
	void SetEnemyAP();

	// AP 증가 시키는 함수
	int32 GetAP(int32 ap);
	// AP 소비하는 함수
	int32 ConsumeAP(int32 ap, int32 amount);
	// 스킬이 사용한지 체크하는 함수
	bool CanUseSkill(int32 ap, int32 cost);
	//--------------------Turn System--------------------
	// currentActionPoints = maxActionPoints;
	// UI 안내용	"플레이어 턴입니다" UI 출력
	// 페이즈 한정 버프/디버프	플레이어 페이즈 시작 시 체력 회복 등
	// 전체 행동 제한	모든 유닛이 행동 못하게 하고 Cutscene 등 삽입
	// AI 판단	적 페이즈에만 그룹 AI 로직 실행
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Phase")
	ATurnManager* turnManager;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Phase")
	TSubclassOf<ATurnManager> turnManagerFactory;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Phase")
	TArray<ABaseBattlePawn*> unitQueue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phase")
	TSubclassOf<ABaseBattlePawn> unitFactory;


	
	// 유닛 첫 큐 세팅
	void SetUnitQueue();
	// 유닛 Array 큐 처럼 사용하기 위한 업데이트
	void SortUnitQueue();
	// 전투 시작
	void StartBattle();
	// 플레이어 턴 시작
	void StartPlayerPhase();
	// 플레이어 턴 종료
	void EndPlayerPhase();
	// 적 턴 시작
	void StartEnemyPhase();
	// 적 턴 종료
	void EndEnemyPhase();
};
