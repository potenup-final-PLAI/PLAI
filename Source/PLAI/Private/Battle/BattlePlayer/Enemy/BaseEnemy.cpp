// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/BattlePlayer/Enemy/BaseEnemy.h"

#include "GridTile.h"
#include "GridTileManager.h"
#include "Enemy/BattleEnemyAnimInstance.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BattlePlayer.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetRelativeLocationAndRotation(FVector(0, 0, -100), FRotator(0, -90, 0));
	
	// Mesh Setting
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("'/Game/Wood_Monster/CharacterParts/Meshes/SK_wood_giant_01_a.SK_wood_giant_01_a'"));
	if (tempMesh.Succeeded())
	{
		meshComp->SetSkeletalMesh(tempMesh.Object);
	}
	
	// Animation Instance 세팅
	// ConstructorHelpers::FClassFinder<UAnimInstance> tempAnimInstance(TEXT("'/Game/JS/Blueprints/Animation/ABP_BattleEnemy.ABP_BattleEnemy_C'"));
	// if (tempAnimInstance.Succeeded())
	// {
	// 	meshComp->SetAnimInstanceClass(tempAnimInstance.Class);
	// }
	
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	// GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &ABaseBattlePawn::TryInitStatus, 0.1f, true);
}

void ABaseEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	enemyAnim = Cast<UBattleEnemyAnimInstance>(meshComp->GetAnimInstance());
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent(
	UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseEnemy::MoveToPlayer(AGridTile* player, AGridTileManager* tileManager)
{
	if (!tileManager)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("BaseEnemy : MoveToPlayer - tileManager is nullptr"));
		return;
	}
	if (!player && !goalTile)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("BaseEnemy : MoveToPlayer - player/goalTile is nullptr"));
		return;
	}
	if (currentTile == goalTile)
	{
		currentActionMode = EActionMode::None;
		if (enemyAnim)
		{
				
			enemyAnim->actionMode = currentActionMode;
			UE_LOG(LogTemp, Warning, TEXT("Processing anim actionMode Update !! %s"), *UEnum::GetValueAsString(enemyAnim->actionMode));
		}
		OnTurnEnd();
		UE_LOG(LogTemp, Warning, TEXT("BaseEnemy : MoveToPlayer - currentTile == goalTile"));
		return;
	}
	InitValues();

	if (player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target Not Nullptr"));
		goalTile = tileManager->FindCurrentTile(player->GetActorLocation());
	}
	UE_LOG(LogTemp, Warning, TEXT("Target is Valid "));
	startTile = tileManager->FindCurrentTile(GetActorLocation());

	if (!IsValid(startTile) || !IsValid(goalTile))
	{
		UE_LOG(LogTemp, Warning, TEXT("Is Not Valid startTile or goalTile"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Is Valid startTile, goalTile"));
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
		float dist = FVector::DistSquared(player->GetActorLocation(),
		                                  GetActorLocation());
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
		if (ABattlePlayer* detectedPlayer = Cast<ABattlePlayer>(
			overlap.GetActor()))
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
		UE_LOG(LogTemp, Warning, TEXT("actionRequest not for this enemy: %s"),
		       *this->GetName());
		return;
	}

	// 배열이 비어 있다면 return
	if (actionRequest.actions.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("ProcessAction: actions array is empty"));
		OnTurnEnd();
		return;
	}

	const FBattleAction& Action = actionRequest.actions[0];
	UE_LOG(LogTemp, Warning, TEXT("Processing action for: %s"),
	       *actionRequest.current_character_id);

	if (actionRequest.actions.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No actions provided"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("actions"));
	}

	// 1. 이동 처리
	if (Action.move_to.Num() == 2)
	{
		// enemy actionmode 업데이트
		currentActionMode = EActionMode::Move;
		if (auto* enemy = Cast<ABaseEnemy>(this))
		{
			if (enemyAnim)
			{
				
				enemyAnim->actionMode = currentActionMode;
				UE_LOG(LogTemp, Warning, TEXT("Processing anim actionMode Update !! %s"), *UEnum::GetValueAsString(enemyAnim->actionMode));
			}
		}
		
		int32 targetX = Action.move_to[0];
		int32 targetY = Action.move_to[1];

		// GridTileManager 통해 목표 타일 찾기
		if (gridTileManager)
		{
			AGridTile* goal = gridTileManager->GetTile(targetX, targetY);
			if (goal)
			{
				goalTile = goal; // ✅ 기존 goalTile 변수에 대입
				if (goalTile)
				{
					UE_LOG(LogTemp, Warning, TEXT("goalTile : %s"),
					       *goal->GetName());
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("goalTile is nullptr"));
				}
				MoveToPlayer(goalTile, gridTileManager); // ✅ 타겟 null로 넘기고 이동만 실행
			}
		}
	}

	// 2. 스킬 처리 (예: 타격이면 공격)
	if (Action.skill == TEXT("타격"))
	{
		attackTarget = FindUnitById(Action.target_character_id);
		bWantsToAttack = true;
		bStartMontage = true;
	}

	// 3. 이동/행동력 소진 후 턴 종료
	if (Action.remaining_ap <= 0 && Action.remaining_mov <= 0)
	{
		FTimerHandle timerHandle;
		GetWorld()->GetTimerManager().SetTimer(timerHandle,FTimerDelegate::CreateLambda([=, this]()
		{
			OnTurnEnd();
		}),1.0f,false);
	}
}

ABaseBattlePawn* ABaseEnemy::FindUnitById(const FString& Id)
{
	TArray<AActor*> FoundUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),
	                                      ABaseBattlePawn::StaticClass(),
	                                      FoundUnits);

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
