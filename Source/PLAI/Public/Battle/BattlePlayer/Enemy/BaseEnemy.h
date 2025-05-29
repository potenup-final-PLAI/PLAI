// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseBattlePawn.h"
#include "BattleEnemyAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "BaseEnemy.generated.h"

UCLASS()
class PLAI_API ABaseEnemy : public ABaseBattlePawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(
		class UInputComponent* PlayerInputComponent) override;
	
	//--------------------Status API 전송용 구조체 변수--------------------
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Battle")
	FBaseStatus enemyStatus;

	//--------------------LifeState--------------------
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Battle")
	ELifeState enemyLifeState;
	
	//-------------enemy Settings--------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class USkeletalMeshComponent* meshComp;

	//------------------Anim Instance--------------------------
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Anim)
	class UBattleEnemyAnimInstance* enemyAnim;
	
	//--------------Test---------------------
	void MoveToPlayer(class AGridTile* targetPlayerTile);
	class ABattlePlayer* FindClosestPlayer(TArray<class ABattlePlayer*>& allPlayers);

	void FindAndAttackPlayer();

	// 받은 데이터로 enemy 움직임
	void ProcessAction(const FActionRequest& actionRequest);
	void ActionMove(const TArray<int32>& actionMove);
	// enemy 스킬리스트
	TMap<FString, EActionMode> ActionMap;

	void InitActionMap();
	void EnemyActionList(const FString& actionName);
	// ID 찾기
	ABaseBattlePawn* FindUnitById(const FString& Id);

	// 현재 AP로 사용 가능한지 체크
	bool TryConsumeAP(int32 amount);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_EnemyTile(class AGridTile* enemyTile);

	UFUNCTION(Server, Reliable)
	void ServerRPC_UpdateEnemyAnim(EActionMode mode);
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastRPC_UpdateEnemyAnim(EActionMode mode);

};

