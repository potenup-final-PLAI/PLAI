// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Enemy/EnemyBattleState.h"
#include "GameFramework/Pawn.h"
#include "Player/BattlePlayerState.h"
#include "BaseBattlePawn.generated.h"

UCLASS()
class PLAI_API ABaseBattlePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseBattlePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//-------------------기본 콜리전 세팅----------------------
	UPROPERTY(EditAnywhere)
	class UBoxComponent* boxComp;
	UPROPERTY(EditAnywhere)
	class UCameraComponent* cameraComp;

	//--------------------AP System--------------------
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Phase")
	int32 maxActionPoints = 5;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Phase")
	int32 curAP = 0;


	// Player AP 세팅
	void SetAP()
	{
		curAP = 0;
	}

	// AP 증가 시키는 함수
	void GetAP()
	{
		if (curAP < maxActionPoints)
		{
			curAP++;
		}
	}

	// 스킬이 사용한지 체크하는 함수
	bool CanUseSkill(int32 cost)
	{
		// 호출하는 대상의 AP를 검사해서 크다면 스킬 실행
		return curAP >= cost;
	}

	// AP 소비하는 함수
	bool CanConsumeAP(int32 cost)
	{
		if (!CanUseSkill(cost))
		{
			UE_LOG(LogTemp, Warning, TEXT("Not enough AP"));
			return false;
		}
		// 호출하는 대상에 ap를 받아서 AP 증감처리 
		curAP = FMath::Max(0, curAP - cost);
		// 처리된 ap를 다시 호출한 대상 AP에 업데이트
		return true;
	}

	//----------Speed State-------------
	int32 speed = 0;
	//------------Turn System-----------------
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ATurnManager> turnManagerFactory;
	UPROPERTY(EditAnywhere)
	class ATurnManager* turnManager;
	void OnTurnStart();
	void OnTurnEnd();

	//------------Skill System-----------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Skill")
	EActionMode currentActionMode = EActionMode::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bBaseAttack = true;

	// 플레이어 스킬들
	void PlayerMove(FHitResult& hitInfo);
	void PlayerBaseAttack(FHitResult& hitInfo);
	void PlayerParalysis(FHitResult& hitInfo);
	void PlayerPoison(FHitResult& hitInfo);
	void PlayerVulnerable(FHitResult& hitInfo);
	void PlayerWeaking(FHitResult& hitInfo);
	void PlayerFatal(FHitResult& hitInfo);
	void PlayerRupture(FHitResult& hitInfo);
	void PlayerRoar(FHitResult& hitInfo);
	void PlayerBattleCry(FHitResult& hitInfo);

	// 대미지 전달 함수
	void ApplyAttack(ABaseBattlePawn* targetUnit,
	                 EActionMode attackType = EActionMode::None);

	// 상태이상과 지속 턴 
	TMap<EStatusEffect, int32> activeStatusEffects;

	// 상태이상 관리 함수
	void AddStatusEffect(EStatusEffect newEffect, int32 duration);
	void ApplyStatusEffect();
	void HandleStateusEffect(EStatusEffect effect);

	// 약화 상태이상 처리 함수
	// player 상태 처리
	void WeakeningProcess(ABattlePlayerState* playerState);
	void VulnerableProcess(ABattlePlayerState* playerState);
	void AngryProcess(ABattlePlayerState* playerState);
	void BleedingProcess(ABattlePlayerState* playerState);
	// enemy 상태 처리
	void WeakeningEnemyProcess(UEnemyBattleState* enemyState);
	void VulnerableEnemyProcess(UEnemyBattleState* enemyState);
	void AngryEnemyProcess(UEnemyBattleState* enemyState);
	void BleedingEnemyProcess(UEnemyBattleState* enemyState);

	//---------------TEST-------------------
	// Player 움직임
	// UI 눌렀을 때 그쪽으로 이동하도록
	UPROPERTY(EditAnywhere)
	class AGridTile* targetTile;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Test)
	TSubclassOf<class AGridTileManager> TileManagerFactory;

	// PlayerState 부분
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Test)
	class ABattlePlayerState* battlePlayerState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Test)
	TArray<FString> playerSkills = {
		TEXT("타격"),
		TEXT("마비의 일격"),
		TEXT("맹동 공격"),
		TEXT("취약 타격"),
		TEXT("약화의 일격"),
		TEXT("치명 일격"),
		TEXT("파열 참격"),
		TEXT("포효"),
		TEXT("전투의 외침")
	};

	// Enemy State 부분
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Test)
	class UEnemyBattleState* enemybattleState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Test)
	TArray<FString> enemySkills = {
		TEXT("타격"),
		TEXT("마비의 일격"),
		TEXT("몸통 박치기"),
		TEXT("맹동 공격"),
		TEXT("취약 타격"),
		TEXT("약화의 일격"),
		TEXT("치명 일격"),
		TEXT("파열 참격"),
		TEXT("대지 가르기"),
		TEXT("독침 찌르기"),
		TEXT("이빨 물기"),
		TEXT("날카로운 발톱"),
		TEXT("단단한 갑각"),
		TEXT("생존 본능"),
		TEXT("유연한 자세"),
		TEXT("전투 준비"),
		TEXT("방어 지휘"),
		TEXT("포효"),
		TEXT("광란 유도"),
		TEXT("전투의 외침")
	};
	// Enemy State Set
	void InitEnemyState();

	// 마우스 클릭 했을 때 처리 하는 부분
	void OnMouseLeftClick();
	void AddOpenByOffset(FIntPoint offset);

	// httpActor 담을 변수 선언
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Test)
	TSubclassOf<class ABattleHttpActor> httpActorFactory;


	//-------------Get Damage-----------------------
	void GetDamage(ABaseBattlePawn* unit, int32 damage);

	//------------Move System-----------------
	// 이 변수에 들어있는 Block 들을 기준으로 상, 하, 좌, 우 검색 후 Cost 구해야 한다.
	UPROPERTY(EditAnywhere)
	TArray<class AGridTile*> openArray;
	// 검색이 다 끝난 타일
	UPROPERTY(EditAnywhere)
	TArray<class AGridTile*> closedArray;
	// 시작 타일
	UPROPERTY(EditAnywhere)
	class AGridTile* startTile;
	// 목적지 타일
	UPROPERTY(EditAnywhere)
	class AGridTile* goalTile;
	// 현재 체크하고 있는 block
	UPROPERTY(EditAnywhere)
	class AGridTile* currentTile;

	// 움직임 플래그
	bool bIsMoveMode = false;

	// UI에서 이동을 눌렀을 때 범위 활성화
	void PathFind();
	void BuildPath();
	void AddOpenArray(FVector dir);

	//--------------Unit Move-------------------
	// 이동 경로 저장 Array
	UPROPERTY(EditAnywhere)
	TArray<AGridTile*> pathArray;
	// 현재 경로 인덱스
	UPROPERTY(EditAnywhere)
	int32 currentPathIndex = 0;
	// 이동 중인지 체크
	UPROPERTY(EditAnywhere)
	bool bIsMoving = false;
	// 이동 속도
	UPROPERTY(EditAnywhere)
	float moveSpeed = 300.0f;

	// 이동력 테스트 코드
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 moveRange = 0;

	// 골 위치를 클릭 했을 때 그쪽으로 이동
	void OnMoveEnd();
	void InitValues();


	//------------- UI --------------
	//-------------Unit 이름, HP, Armor 세팅------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	class UWidgetComponent* battleUnitStateComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	class ABaseBattlePawn* lastHoveredPawn;

	void BillboardBattleUnitStateUI();
	void OnMouseHover();
	//-----------Player Anim Instace---------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Anim)
	class ABaseEnemy* targetEnemy;

	//-----------Enemy Anim Instace---------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Anim)
	class ABattlePlayer* targetPlayer;

	//-------------Rotation 처리--------------------
	bool bWantsToAttack = false;
	bool bStartMontage = false;
	ABaseBattlePawn* attackTarget;

	//------------Enemy Turn 여러 번 호출 방지--------
	bool bTurnEnded = false;

	//-------------Damage Actor Widget-----------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageUI")
	TSubclassOf<class AWorldDamageUIActor> damageUIFactory;
};
