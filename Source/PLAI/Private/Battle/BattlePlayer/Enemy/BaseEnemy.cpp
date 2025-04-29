// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/BattlePlayer/Enemy/BaseEnemy.h"

#include "BasePlayerState.h"
#include "GridTile.h"
#include "GridTileManager.h"
#include "Engine/OverlapResult.h"
#include "Player/BattlePlayer.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &ABaseBattlePawn::TryInitStatus, 0.1f, true);
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseEnemy::MoveToPlayer(ABattlePlayer* player, AGridTileManager* tileManager)
{
	if (!player || !tileManager) return;
	
	InitValues();
	goalTile = tileManager->FindCurrentTile(player->GetActorLocation());
	startTile = tileManager->FindCurrentTile(GetActorLocation());

	if (!IsValid(startTile) || !IsValid(goalTile)) return;

	startTile->sCostValue = 0;
	openArray.Add(startTile);

	PathFind();
}

ABattlePlayer* ABaseEnemy::FindClosestPlayer(TArray<ABattlePlayer*>& allPlayers)
{
	ABattlePlayer* closet = nullptr;
	float minDist = TNumericLimits<float>::Max();

	for (ABattlePlayer* player : allPlayers)
	{
		float dist = FVector::DistSquared(player->GetActorLocation(), GetActorLocation());
		if (dist < minDist)
		{
			minDist = dist;
			closet = player;
		}
	}
	
	return closet;
}

void ABaseEnemy::FindAndAttackPlayer()
{
	FVector centerPos = GetActorLocation();
	float detectRadius = 200.0f;
	// Debug sphere로 주변 탐색
	DrawDebugSphere(
		GetWorld(),
		centerPos,
		detectRadius,
		16,
		FColor::Red,
		false,
		0.1f
		);
	TArray<FOverlapResult> overlaps;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);

	GetWorld()->OverlapMultiByChannel(
		overlaps,
		centerPos,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(detectRadius),
		queryParams
		);
	for (auto& overlap : overlaps)
	{
		// 탐색 했을 때 그 객체가 Player라면
		if (ABattlePlayer* detectedPlayer = Cast<ABattlePlayer>(overlap.GetActor()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Detected Player"));
			// 플레이어가 있다면 공격
			this->BaseAttack(detectedPlayer);
			break;
		}
	}
	
	// 끝났으면 턴 종료 처리
	OnTurnEnd();
}
