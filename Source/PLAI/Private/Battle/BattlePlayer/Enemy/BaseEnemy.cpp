// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/BattlePlayer/Enemy/BaseEnemy.h"

#include "GridTile.h"
#include "GridTileManager.h"
#include "Battle/UI/BattleUnitStateUI.h"
#include "Components/WidgetComponent.h"
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
	meshComp->SetRelativeLocationAndRotation(FVector(0, 0, -100),FRotator(0, -90, 0));
	meshComp->bReceivesDecals = false;
	
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	// GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &ABaseBattlePawn::TryInitStatus, 0.1f, true);
	InitActionMap();
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
			UE_LOG(LogTemp, Warning,TEXT("Processing anim actionMode Update !! %s"),*UEnum::GetValueAsString(enemyAnim->actionMode));
		}
		OnTurnEnd();
		UE_LOG(LogTemp, Warning,
		       TEXT("BaseEnemy : MoveToPlayer - currentTile == goalTile"));
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
			this->EnemyApplyAttack(detectedPlayer, EActionMode::Poison);
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
	if (actionRequest.action.target_character_id == "")
	{
		UE_LOG(LogTemp, Error, TEXT("ProcessAction: actions array is empty"));
		OnTurnEnd();
		return;
	}
	
	const FBattleAction& action = actionRequest.action;
	UE_LOG(LogTemp, Warning, TEXT("Processing action for: %s"),
	       *actionRequest.current_character_id);
	
	
	// 3. API 행동 이유 출력
	if (action.reason != "")
	{
		FString reason = action.reason;
		this->battleUnitStateComp->SetDrawSize(FVector2d(150, 100));
		this->battleUnitStateUI->ShowAPIReasonUI();
		this->battleUnitStateUI->SetAPIReason(reason);

		FTimerHandle actionReasonTimerHandle;
		GetWorld()->GetTimerManager().ClearTimer(actionReasonTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(actionReasonTimerHandle,FTimerDelegate::CreateLambda([this]()
		{
			this->battleUnitStateUI->ShowBaseUI();
			this->battleUnitStateComp->SetDrawSize(FVector2D(50, 10));
		}), 3.0f, false);
	}
	
	// 1. 이동 처리
	if (action.move_to.Num() == 2)
	{
		// enemy actionmode 업데이트
		if (auto* enemy = Cast<ABaseEnemy>(this))
		{
			enemy->currentActionMode = EActionMode::Move;
			if (enemybattleState->move_Range) SeeMoveRange(this->enemybattleState->move_Range);
			else UE_LOG(LogTemp, Warning, TEXT("Enemybattle not State"));
			
			if (enemyAnim)
			{
				enemyAnim->actionMode = currentActionMode;
				UE_LOG(LogTemp, Warning,TEXT("Processing anim actionMode Update !! %s"), *UEnum::GetValueAsString(enemyAnim->actionMode));
			}
		}

		int32 targetX = action.move_to[0];
		int32 targetY = action.move_to[1];

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
				MoveToPlayer(goalTile, gridTileManager);
				// ✅ 타겟 null로 넘기고 이동만 실행
			}
		}
	}

	// 2. 스킬 처리 (예: 타격이면 공격)
	if (action.skill == TEXT("타격"))
	{
		attackTarget = FindUnitById(action.target_character_id);
		bWantsToAttack = true;
		bStartMontage = true;
	}

	
	// 3. 이동/행동력 소진 후 턴 종료
	if (action.remaining_ap <= 0 && action.remaining_mov <= 0)
	{
		FTimerHandle timerHandle;
		GetWorld()->GetTimerManager().SetTimer(timerHandle,FTimerDelegate::CreateLambda([=, this]()
		{
			OnTurnEnd();
		}), 1.0f, false);
	}
}

void ABaseEnemy::InitActionMap()
{
	ActionMap.Add(TEXT("타격"), EActionMode::BaseAttack);
	ActionMap.Add(TEXT("마비의 일격"), EActionMode::Paralysis);
	ActionMap.Add(TEXT("몸통 박치기"), EActionMode::Charge);
	ActionMap.Add(TEXT("맹독 공격"), EActionMode::Poison);
	ActionMap.Add(TEXT("취약 타격"), EActionMode::Vulnerable);
	ActionMap.Add(TEXT("약화의 일격"), EActionMode::Weakening);
	ActionMap.Add(TEXT("치명 일격"), EActionMode::Fatal);
	ActionMap.Add(TEXT("파열 참격"), EActionMode::Rupture);
	ActionMap.Add(TEXT("대지 가르기"), EActionMode::Unique_EarthBreak);
	ActionMap.Add(TEXT("독침 찌르기"), EActionMode::Unique_PoisonSting);
	ActionMap.Add(TEXT("단단한 갑각"), EActionMode::Unique_Harden);
	ActionMap.Add(TEXT("생존 본능"), EActionMode::Unique_Instinct);
	ActionMap.Add(TEXT("유연한 자세"), EActionMode::Unique_Flexibility);
	ActionMap.Add(TEXT("전투 준비"), EActionMode::BattleCry);
	ActionMap.Add(TEXT("방어 지휘"), EActionMode::Unique_DefenseOrder);
	ActionMap.Add(TEXT("포효"), EActionMode::Roar);
	ActionMap.Add(TEXT("광란 유도"), EActionMode::Unique_Enrage);
	ActionMap.Add(TEXT("전투의 외침"), EActionMode::BattleCry);
}

void ABaseEnemy::EnemyActionList(const FString& actionName)
{
	if (!ActionMap.Contains(actionName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Unknown action: %s"), *actionName);
		return;
	}

	EActionMode action = ActionMap[actionName];

	switch (action)
	{
	case EActionMode::BaseAttack:
		UE_LOG(LogTemp, Warning, TEXT("타격"));
		break;

	case EActionMode::Paralysis:
		UE_LOG(LogTemp, Warning, TEXT("마비의 일격"));
		break;

	case EActionMode::Charge:
		UE_LOG(LogTemp, Warning, TEXT("몸통 박치기"));
		break;

	case EActionMode::Poison:
		UE_LOG(LogTemp, Warning, TEXT("맹독 공격"));
		break;

	case EActionMode::Vulnerable:
		UE_LOG(LogTemp, Warning, TEXT("취약 타격"));
		break;

	case EActionMode::Weakening:
		UE_LOG(LogTemp, Warning, TEXT("약화의 일격"));
		break;

	case EActionMode::Fatal:
		UE_LOG(LogTemp, Warning, TEXT("치명 일격"));
		break;

	case EActionMode::Rupture:
		UE_LOG(LogTemp, Warning, TEXT("파열 참격"));
		break;

	case EActionMode::Unique_EarthBreak:
		UE_LOG(LogTemp, Warning, TEXT("대지 가르기"));
		break;

	case EActionMode::Unique_PoisonSting:
		UE_LOG(LogTemp, Warning, TEXT("독침 찌르기"));
		break;

	case EActionMode::Unique_Harden:
		UE_LOG(LogTemp, Warning, TEXT("단단한 갑각"));
		break;

	case EActionMode::Unique_Instinct:
		UE_LOG(LogTemp, Warning, TEXT("생존 본능"));
		break;

	case EActionMode::Unique_Flexibility:
		UE_LOG(LogTemp, Warning, TEXT("유연한 자세"));
		break;

	case EActionMode::BattleCry:
		UE_LOG(LogTemp, Warning, TEXT("전투 외침"));
		break;

	case EActionMode::Unique_DefenseOrder:
		UE_LOG(LogTemp, Warning, TEXT("방어 지휘"));
		break;

	case EActionMode::Roar:
		UE_LOG(LogTemp, Warning, TEXT("포효"));
		break;

	case EActionMode::Unique_Enrage:
		UE_LOG(LogTemp, Warning, TEXT("광란 유도"));
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("정의되지 않은 행동"));
		break;
	}
}

ABaseBattlePawn* ABaseEnemy::FindUnitById(const FString& Id)
{
	TArray<AActor*> FoundUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ABaseBattlePawn::StaticClass(),FoundUnits);

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

bool ABaseEnemy::TryConsumeAP(int32 amount)
{
	if (!enemybattleState->CanConsumeAP(amount))
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
		return false;
	}
	return true;
}


