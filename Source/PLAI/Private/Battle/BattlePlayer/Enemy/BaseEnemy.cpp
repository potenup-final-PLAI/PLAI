// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/BattlePlayer/Enemy/BaseEnemy.h"

#include "BasePlayerState.h"
#include "GridTile.h"
#include "GridTileManager.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
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
	
	// GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &ABaseBattlePawn::TryInitStatus, 0.1f, true);
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
	if (!tileManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("BaseEnemy : MoveToPlayer - tileManager is nullptr"));
		return;
	}
	if (!player && !goalTile)
	{
		UE_LOG(LogTemp, Warning, TEXT("BaseEnemy : MoveToPlayer - player/goalTile is nullptr"));
		return;
	}
	
	InitValues();

	if (player)
	{
		goalTile = tileManager->FindCurrentTile(player->GetActorLocation());
	}
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
			this->ApplyAttack(detectedPlayer, EActionMode::Poison);
			break;
		}
	}
	
	// 끝났으면 턴 종료 처리
	OnTurnEnd();
}

void ABaseEnemy::ProcessAction(const FActionRequest& actionRequest)
{
	// 본인이 서버에서 지시된 캐릭터인지 확인
	if (actionRequest.current_character_id != this->GetName())
	{
		UE_LOG(LogTemp, Warning, TEXT("actionRequest not for this enemy: %s"), *this->GetName());
		return;
	}

	if (actionRequest.actions.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No actions provided"));
		return;
	}

	const FBattleAction& Action = actionRequest.actions[0];

	// 1. 이동 처리
	if (Action.move_to.Num() == 2)
	{
		int32 targetX = Action.move_to[0];
		int32 targetY = Action.move_to[1];

		// 예시: GridTileManager 통해 목표 타일 찾기
		if (AGridTileManager* tileManager = Cast<AGridTileManager>(
			UGameplayStatics::GetActorOfClass(GetWorld(), TileManagerFactory)))
		{
			AGridTile* goal = tileManager->GetTile(targetX, targetY);
			if (goal)
			{
				goalTile = goal; // ✅ 기존 goalTile 변수에 대입
				MoveToPlayer(nullptr, tileManager); // ✅ 타겟 null로 넘기고 이동만 실행
			}
		}
	}

	// 2. 스킬 처리 (예: 타격이면 공격)
	if (Action.skill == TEXT("타격"))
	{
		ABaseBattlePawn* Target = FindUnitById(Action.target_character_id);
		if (Target)
		{
			ApplyAttack(Target, EActionMode::BaseAttack); // 공격 실행
		}
	}

	// 3. 이동/행동력 소진 후 턴 종료
	if (Action.remaining_ap <= 0 && Action.remaining_mov <= 0)
	{
		OnTurnEnd();
	}
}
ABaseBattlePawn* ABaseEnemy::FindUnitById(const FString& Id)
{
	TArray<AActor*> FoundUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseBattlePawn::StaticClass(), FoundUnits);

	for (AActor* Actor : FoundUnits)
	{
		if (ABaseBattlePawn* Unit = Cast<ABaseBattlePawn>(Actor))
		{
			if (Unit->GetName() == Id)
			{
				return Unit;
			}
		}
	}
	return nullptr;
}
