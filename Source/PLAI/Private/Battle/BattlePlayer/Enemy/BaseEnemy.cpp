// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/BattlePlayer/Enemy/BaseEnemy.h"

#include "GridTile.h"
#include "GridTileManager.h"
#include "Battle/TurnSystem/PhaseManager.h"
#include "Battle/Util/DebugHeader.h"
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
	enemyAnim = Cast<UBattleEnemyAnimInstance>(meshComp->GetAnimInstance());
	InitActionMap();
}

void ABaseEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	
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

void ABaseEnemy::MoveToPlayer(AGridTile* targetPlayerTile)
{
	if (!goalTile)
	{
		UE_LOG(LogTemp, Warning, TEXT("BaseEnemy : MoveToPlayer - goalTile is nullptr"));
		return;
	}
	if (!currentTile)
	{
		UE_LOG(LogTemp, Error, TEXT("ABaseEnemy::MoveToPlayer : !currentTile"));
		return;
	}

	// 현재 타일과 골 타일이 같다면
	NET_PRINTLOG(TEXT("Enemy %s, currentTile %s, goalTile %s"), *GetActorNameOrLabel(), *currentTile->GetActorNameOrLabel(), *goalTile->GetActorNameOrLabel());
	
	if (currentTile == goalTile)
	{
		if (enemyAnim)
		{
			currentActionMode = EActionMode::None;
			MultiCastRPC_UpdateEnemyAnim(currentActionMode);
			UE_LOG(LogTemp, Warning,TEXT("Processing anim actionMode Update !! %s"),*UEnum::GetValueAsString(enemyAnim->actionMode));
		}

		// phaseManager->turnManager->OnTurnEnd();

		UE_LOG(LogTemp, Warning, TEXT("BaseEnemy : MoveToPlayer - currentTile == goalTile"));
		return;
	}
	InitValues();
	
	goalTile = phaseManager->gridTileManager->FindCurrentTile(targetPlayerTile->GetActorLocation());
	
	startTile = phaseManager->gridTileManager->FindCurrentTile(GetActorLocation());
	
	// 현재 위치를 startTile로 설정
	// startTile = currentTile;
	
	if (!IsValid(startTile))
	{
		UE_LOG(LogTemp, Warning, TEXT("Is Not Valid startTile"));
		return;
	}
	NET_PRINTLOG(TEXT("Enemy %s, currentTile %s, goalTile %s, startTile %s"), *GetActorNameOrLabel(), *currentTile->GetActorNameOrLabel(), *goalTile->GetActorNameOrLabel(), *startTile->GetActorNameOrLabel());
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
		float dist = FVector::DistSquared(player->GetActorLocation(),GetActorLocation());
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
	// DrawDebugSphere(
	// 	GetWorld(),
	// 	centerPos,
	// 	detectRadius,
	// 	16,
	// 	FColor::Red,
	// 	false,
	// 	0
	// );
	
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
			// 플레이어가 있다면 대상에 추가
			targetPlayer = detectedPlayer;
			attackTarget = targetPlayer;
			FString s = " ";
			NET_PRINTLOG(TEXT("attackPlayer %s, targetPlayer %s"), attackTarget != nullptr ? *attackTarget->GetActorNameOrLabel() : *s, targetPlayer != nullptr ? *targetPlayer->GetActorNameOrLabel() : *s);
			break;
		}
	}
}

void ABaseEnemy::ProcessAction(const FActionRequest& actionRequest)
{
	// 본인이 서버에서 지시된 캐릭터인지 확인
	if (actionRequest.current_character_id != this->GetName())
	{
		UE_LOG(LogTemp, Warning, TEXT("actionRequest not for this enemy: %s"), *this->GetName());
		return;
	}

	// 0. 턴 종료하라고 오면 턴 종료
	
	
	const FBattleAction& action = actionRequest.action;
	UE_LOG(LogTemp, Warning, TEXT("Processing action for: %s"), *actionRequest.current_character_id);

	// 1. API로 받은 Enemy 이동
	if (actionRequest.action.move_to.Num() != 0)
	{
		// Player Move 시작
		ActionMove(actionRequest.action.move_to);	
	}
	
	// 2. API 행동 이유 출력
	if (action.dialogue != "")
	{
		FString dialogue = action.dialogue;
		UE_LOG(LogTemp, Warning, TEXT("action.dialogue != 공백 아님 "));
		ShowDialoge(dialogue);
	}
	
	// 3. 스킬 처리 (예: 타격이면 공격)
	if (action.skill != "")
	{
		EnemyActionList(actionRequest.action.skill);
		// attackTarget = FindUnitById(action.target_character_id);
		// targetPlayer = Cast<ABattlePlayer>(attackTarget);
		// FString s = " ";
		// NET_PRINTLOG(TEXT("attackPlayer %s, targetPlayer %s"), attackTarget != nullptr ? *attackTarget->GetActorNameOrLabel() : *s, targetPlayer != nullptr ? *targetPlayer->GetActorNameOrLabel() : *s);
		bWantsToAttack = true;
		bStartMontage = true;
	}

	// 턴 종료 호출
	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(timerHandle, FTimerDelegate::CreateLambda([&]()
	{
		phaseManager->turnManager->OnTurnEnd();	
	}), 2.5f, false);
}

void ABaseEnemy::ActionMove(const TArray<int32>& actionMove)
{
	// 1. 이동 처리
	
	int32 targetX = actionMove[0];
	int32 targetY = actionMove[1];

	// GridTileManager 통해 목표 타일 찾기
	if (gridTileManager)
	{
		goalTile = gridTileManager->GetTile(targetX, targetY);
		// FString netLog = FString::Printf(TEXT("%s(%s) : myTile(%d,%d) goalTile(%d,%d)"), *GetName(), *MyName, currentTile->gridCoord.X, currentTile->gridCoord.Y, targetX, targetY);
		// phaseManager->AddNetLog(netLog);
		
		// NET_PRINTLOG(TEXT("ABaseEnemy::ActionMove : %s, goalTile : %s"), *netLog, goalTile ? *goalTile->GetActorNameOrLabel() : TEXT("nullptr"));
		if (goalTile)
		{
			// Player쪽으로 이동 실행
			MoveToPlayer(goalTile);
		}
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
	TArray<AActor*> foundUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABattlePlayer::StaticClass(),foundUnits);

	for (AActor* actor : foundUnits)
	{
		if (ABattlePlayer* unit = Cast<ABattlePlayer>(actor))
		{
			if (unit->MyName == Id)
			{
				return unit;
			}
		}
	}
	return nullptr;
}

bool ABaseEnemy::TryConsumeAP(int32 amount)
{
	if (!this->CanConsumeAP(amount))
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
		return false;
	}
	return true;
}

void ABaseEnemy::MulticastRPC_EnemyTile_Implementation(class AGridTile* enemyTile)
{
	currentTile = enemyTile;
	Multicast_SeeMoveRange_Implementation();

	NET_PRINTLOG(TEXT("enemy %s, enemy->currentTile %s"), *MyName, *currentTile->GetActorNameOrLabel());
}

void ABaseEnemy::ServerRPC_UpdateEnemyAnim_Implementation(EActionMode mode)
{
	MultiCastRPC_UpdateEnemyAnim(mode);
}

void ABaseEnemy::MultiCastRPC_UpdateEnemyAnim_Implementation(EActionMode mode)
{
	if (enemyAnim)
	{
		enemyAnim->actionMode = mode;
		NET_PRINTLOG(TEXT("호출한 객체 : %s, currentActionMode : %s, enemyAnim->actionMode : %s"), *GetActorNameOrLabel(), *UEnum::GetValueAsString(currentActionMode), *UEnum::GetValueAsString(enemyAnim->actionMode));
	}
}