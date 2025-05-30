// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/Util/DebugHeader.h"
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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//------------------Player Status------------------------
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Status")
	int32 hp = 0;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Status")
	int32 attack = 0;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Status")
	int32 defense = 0;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Status")
	int32 resistance = 0;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Status")
	float critical_Rate = 0.0f;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Status")
	float critical_Damage = 0.0f;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Status")
	int32 moveRange = 0;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Status")
	int32 speed = 0;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Status")
	int32 points = 0;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Status")
	TArray<FString> traits = {};
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Status")
	TArray<FString> skills = {};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Status")
	int32 maxHP = 0;
	// Player가 죽었는지 살았는지 상태 체크
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Status")
	ELifeState playerLifeState;
	//-------------------기본 콜리전 세팅----------------------
	UPROPERTY(EditAnywhere)
	class UBoxComponent* boxComp;
	UPROPERTY(EditAnywhere)
	class UCameraComponent* cameraComp;

	//---------------------다른 클래스 포인터----------------------------
	UPROPERTY(EditAnywhere)
	class AUPhaseManager* phaseManager;
	UPROPERTY(EditAnywhere)
	class ATurnManager* turnManager;
	UPROPERTY(EditAnywhere)
	class APlayerController* pc;
	UPROPERTY(EditAnywhere)
	class AGridTileManager* gridTileManager;
	UPROPERTY(EditAnywhere)
	class UWorldGi* gi;
	// UI
	UPROPERTY(EditAnywhere)
	class UBattleUnitStateUI* battleUnitStateUI;
	UPROPERTY(EditAnywhere)
	class ABattleHUD* hud;

	// 다른 클래스 포인터 초기화
	void InitOtherClassPointer();

	UFUNCTION(Server, Reliable)
	void ServerInitOtherClassPointer();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastInitOtherClassPointer();

	//--------------------AP System--------------------
	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadOnly, Category = "Phase")
	int32 maxActionPoints = 5;
	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadOnly, Category = "Phase")
	int32 curAP = 0;

	void InitAPUI();
	UFUNCTION(Client, Reliable)
	void ClientRPC_AddAP(class ABattlePlayer* player);
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastRPC_InitAPUI();
	
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
			NET_PRINTLOG(TEXT("cost : %d, ap : %d "), cost, curAP);
			UE_LOG(LogTemp, Warning, TEXT("Not enough AP"));
			return false;
		}
		// 호출하는 대상에 ap를 받아서 AP 증감처리 
		curAP = FMath::Max(0, curAP - cost);
		// 처리된 ap를 다시 호출한 대상 AP에 업데이트
		return true;
	}
	
	//------------Turn System-----------------
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ATurnManager> turnManagerFactory;


	UFUNCTION(NetMulticast, Reliable)
	void MultiCastRPC_SetBattlePlayerInfoUI();
	//------------Skill System-----------------
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Skill")
	EActionMode currentActionMode = EActionMode::None;

	// UFUNCTION()
	// void OnRep_ActionModeChanged();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	bool bBaseAttack = true;

	// 플레이어 스킬들
	void PlayerMove(FHitResult& hitInfo);
	UFUNCTION(Server, Reliable)
	void Server_UpdateCurrentTile(class AGridTile* tile);
	void PlayerBaseAttack(FHitResult& hitInfo);
	void PlayerPoison(FHitResult& hitInfo);
	void PlayerFatal(FHitResult& hitInfo);
	void PlayerRupture(FHitResult& hitInfo);

	// Player 대미지 전달 함수
	UFUNCTION(Server, Reliable)
	void Server_PlayerApplyAttack(ABaseBattlePawn* targetUnit, EActionMode attackType = EActionMode::None);
	UFUNCTION(NetMulticast, Reliable)
	void PlayerApplyAttack(ABaseBattlePawn* targetUnit, EActionMode attackType = EActionMode::None);
	// Enemy 대미지 전달 함수
	void EnemyApplyAttack(ABaseBattlePawn* targetUnit, EActionMode attackType = EActionMode::None);
	// Damage 계산 함수
	void CalculateAndApplyDamage(ABaseBattlePawn* target, int32 atk, float skillMultiplier, float criticalRate, float criticalDamage);
	
	// 상태이상과 지속 턴 
	TMap<EStatusEffect, int32> activeStatusEffects;

	// 상태이상 관리 함수
	void AddStatusEffect(EStatusEffect newEffect, int32 duration);
	void ApplyStatusEffect();
	void HandleStatusEffect(EStatusEffect effect);

	// 약화 상태이상 처리 함수
	// player 상태 처리
	void WeakeningProcess();
	void VulnerableProcess();
	void AngryProcess();
	void BleedingProcess();
	// enemy 상태 처리
	void WeakeningEnemyProcess(class ABaseEnemy* enemy);
	void VulnerableEnemyProcess(class ABaseEnemy* enemy);
	void AngryEnemyProcess(class ABaseEnemy* enemy);
	void BleedingEnemyProcess(class ABaseEnemy* enemy);

	//---------------TEST-------------------
	// Player 움직임
	// UI 눌렀을 때 그쪽으로 이동하도록
	UPROPERTY(EditAnywhere)
	class AGridTile* targetTile;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Test)
	TSubclassOf<class AGridTileManager> TileManagerFactory;
	
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
		TEXT("맹독 공격"),
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
	// Enemy 성격 세팅
	TArray<FString> enemyTraits = {
		TEXT("강인함"),
		TEXT("잔잔함"),
		TEXT("호전적"),
		TEXT("충동적"),
		TEXT("수비적"),
		TEXT("신중함"),
		TEXT("관찰꾼"),
		TEXT("잔인함"),
		TEXT("겁쟁이"),
		TEXT("허세꾼"),
		TEXT("교란꾼"),
		TEXT("파괴적"),
		TEXT("협동적"),
		TEXT("용감함"),
		TEXT("조화적"),
		TEXT("고립적"),
		TEXT("지능적"),
		TEXT("냉정함"),
		TEXT("원한꾼"),
		TEXT("민첩함")
	};
	// Enemy 스텟 세팅
	void InitEnemyState();
	// Enemy 성격 세팅
	void InitEnemyTraits();
	// Enemy 기술 세팅
	void InitEnemySkills();
	void ApplyTraitModifiers(class ABaseEnemy* enemy);
	// 마우스 클릭 했을 때 처리 하는 부분
	void OnMouseLeftClick();
	void AddOpenByOffset(FIntPoint offset);

	// httpActor 담을 변수 선언
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Test)
	TSubclassOf<class ABattleHttpActor> httpActorFactory;


	//-------------Get Damage-----------------------
	void GetDamage(ABaseBattlePawn* unit, int32 damage);
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastRPC_PlayerGetDamage(class ABattlePlayer* player, int32 damage);
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastRPC_EnemyGetDamage(class ABaseEnemy* enemy, int32 damage);
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
	UPROPERTY(Replicated, EditAnywhere)
	class AGridTile* currentTile;

	// 움직임 플래그
	bool bIsMoveMode = false;

	// UI에서 이동을 눌렀을 때 범위 활성화
	void PathFind();
	void BuildPath();
	void AddOpenArray(FVector dir);
	// 이동 범위 보일 수 있도록 주변 타일 색 변경 하는 함
	UPROPERTY(EditAnywhere)
	TArray<AGridTile*> highlightedTiles;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SeeMoveRange();
	void SeeMoveRange(int32 move_Range, TArray<FIntPoint>& tiles);
	void ClearGridTile();
	//--------------Unit Move-------------------
	// 이동 경로 저장 Array
	UPROPERTY(Replicated, EditAnywhere)
	TArray<AGridTile*> pathArray;
	// 현재 경로 인덱스
	UPROPERTY(Replicated, EditAnywhere)
	int32 currentPathIndex = 0;
	// 이동 중인지 체크
	UPROPERTY(Replicated, EditAnywhere)
	bool bIsMoving = false;
	// 이동 속도
	UPROPERTY(Replicated, EditAnywhere)
	float moveSpeed = 300.0f;
	
	UPROPERTY(ReplicatedUsing = OnRep_TargetLoc, EditAnywhere)
	FVector newLoc;

	UFUNCTION()
	void OnRep_TargetLoc();

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastRPC_UpdateNewLoc(const FVector& newLocation);
	//-------------Rotation 처리--------------------
	UPROPERTY(Replicated, EditAnywhere)
	FRotator newRot;
	UPROPERTY(Replicated, EditAnywhere)
	bool bWantsToAttack = false;
	UPROPERTY(Replicated, EditAnywhere)
	bool bStartMontage = false;
	UPROPERTY(Replicated, EditAnywhere)
	FRotator smoothRot;
	UPROPERTY(Replicated, EditAnywhere)
	ABaseBattlePawn* attackTarget;
	
	UPROPERTY(Replicated, EditAnywhere)
	FVector directionToTarget;
	UPROPERTY(Replicated, EditAnywhere)
	FRotator desiredRot;
	UPROPERTY(Replicated, EditAnywhere)
	FRotator targetMeshRot;
	
	// 골 위치를 클릭 했을 때 그쪽으로 이동
	void InitValues();

	// Unit 회전 동기화
	void UnitRotation(class ABaseBattlePawn* unit);
	UFUNCTION(Server, Reliable)
	void ServerRPC_UnitRotation(const FRotator& rot);
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastRPC_UnitRotation(const FRotator& rot);

	// Unit 이동 동기화
	void UnitMove(class ABaseBattlePawn* unit);
	UFUNCTION(Server, Reliable)
	void ServerRPC_UnitMove(const FVector& loc);
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastRPC_UnitMove(const FVector& loc);

	// 공격 하기전 회전
	void UnitAttackBeforeRoatation(class ABaseBattlePawn* unit);

	// Unit 공격
	void UnitAttack(class ABaseBattlePawn* unit);

	// Player 공격 동기화
	UFUNCTION(Server, Reliable)
	void Server_PlayerBaseAttack(class ABattlePlayer* player);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayerBaseAttack(class ABattlePlayer* player);

	// Enemy 공격 동기화
	UPROPERTY(Replicated, EditAnywhere)
	class UAnimMontage* enemyBaseAttack;
	UFUNCTION(Server, Reliable)
	void Server_EnemyBaseAttack(class ABaseEnemy* enemy);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EnemyBaseAttack(class ABaseEnemy* enemy);
	//------------- UI --------------
	//-------------Unit 이름, HP, Armor 세팅------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	class UWidgetComponent* battleUnitStateComp;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = UI)
	class ABaseBattlePawn* lastHoveredPawn;

	void BillboardBattleUnitStateUI();
	void OnMouseHover();
	//------------Print Skill UI --------
	UPROPERTY(Replicated, EditAnywhere)
	FString curSkillName = "";
	
	void SkillNameJudgment(const EActionMode curAction);
	UFUNCTION(Server, Reliable)
	void Server_SkillName(const EActionMode curAction);
	UFUNCTION(NetMulticast, Reliable)
	void MultiCast_SkillName(const EActionMode curAction);
	//-----------Player Anim Instace---------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Anim)
	class ABaseEnemy* targetEnemy;

	//-----------Enemy Anim Instace---------------------
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = Anim)
	class ABattlePlayer* targetPlayer;

	//-------------Enemy Name----------------
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastRPC_SetEnemyName(class ABaseEnemy* enemy);


	UPROPERTY(Replicated)
	FString MyName = TEXT("Unit");

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastRPC_SetMyName(int32 Count);


	//------------Reason UI------------------
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UReasonUI> reasonUIFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMainBattleUI> mainUIFactory;

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastRPC_UpdateReason();
	UFUNCTION(Server, Reliable)
	void ShowDialoge(const FString& dialogue);
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastRPC_ShowDialoge(const FString& dialogue);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastRPC_ClearGridTile();
	
	
};


