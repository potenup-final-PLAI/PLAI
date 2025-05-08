// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseBattlePawn.h"
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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(
		class UInputComponent* PlayerInputComponent) override;


	//--------------Test---------------------
	void MoveToPlayer(class AGridTile* player,
	                  class AGridTileManager* tileManager);
	class ABattlePlayer* FindClosestPlayer(
		TArray<class ABattlePlayer*>& allPlayers);

	void FindAndAttackPlayer();

	// 받은 데이터로 enemy 움직임
	void ProcessAction(const FActionRequest& actionRequest);
	ABaseBattlePawn* FindUnitById(const FString& Id);
};
