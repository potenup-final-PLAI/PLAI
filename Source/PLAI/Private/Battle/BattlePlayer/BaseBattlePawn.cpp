// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/BattlePlayer/BaseBattlePawn.h"

#include "GridTile.h"
#include "GridTileManager.h"
#include "Battle/Http/BattleHttpActor.h"
#include "Battle/TurnSystem/PhaseManager.h"
#include "Battle/TurnSystem/TurnManager.h"
#include "Battle/UI/BattleHUD.h"
#include "Battle/UI/BattlePlayerInfoUI.h"
#include "Battle/UI/BattleUnitStateUI.h"
#include "Battle/UI/MainBattleUI.h"
#include "Battle/UI/ReasonUI.h"
#include "Battle/UI/WorldDamageUI.h"
#include "Battle/UI/WorldDamageUIActor.h"
#include "Battle/Util/DebugHeader.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include "Components/WidgetComponent.h"
#include "Enemy/BaseEnemy.h"
#include "Enemy/BattleEnemyAnimInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "PLAI/Item/GameInstance/WorldGi.h"
#include "Player/BattlePlayer.h"
#include "Player/BattlePlayerAnimInstance.h"

// Sets default values
ABaseBattlePawn::ABaseBattlePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("boxComp"));
	SetRootComponent(boxComp);
	boxComp->SetBoxExtent(FVector(50, 50, 100));

	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("cameraComp"));
	cameraComp->SetupAttachment(RootComponent);
	cameraComp->SetRelativeLocationAndRotation(FVector(-600, 0, 1000),FRotator(-60, 0, 0));

	// 이름, HP, Armor 설정 컴포넌트
	battleUnitStateComp = CreateDefaultSubobject<UWidgetComponent>("BattleUnitStateComp");
	battleUnitStateComp->SetupAttachment(RootComponent);
	battleUnitStateComp->SetDrawSize(FVector2D((0)));
	battleUnitStateComp->SetRelativeLocationAndRotation(FVector(0, 0, 170), FRotator(0));
	battleUnitStateComp->SetGenerateOverlapEvents(false); // 충돌 막기
	battleUnitStateComp->SetVisibility(true);
	battleUnitStateComp->SetCastShadow(false);

	ConstructorHelpers::FClassFinder<UBattleUnitStateUI> tempBattleUnitStateUI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/JS/UI/WBP_BattleUnitState.WBP_BattleUnitState_C'"));
	if (tempBattleUnitStateUI.Succeeded())
	{
		battleUnitStateComp->SetWidgetClass(tempBattleUnitStateUI.Class);
	}

	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ABaseBattlePawn::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
	
	// 사용할 다른 클래스 포인터 초기화
	InitOtherClassPointer();
	
	// UI 생성
	if (battleUnitStateUI)
	{
		battleUnitStateComp->SetDrawSize(FVector2D(50, 10));
		battleUnitStateUI->ShowBaseUI();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BattleUnitStateUI is NULL"));
	}
	
	// AP 세팅
	SetAP();
}

void ABaseBattlePawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseBattlePawn, hp);
	DOREPLIFETIME(ABaseBattlePawn, attack);
	DOREPLIFETIME(ABaseBattlePawn, defense);
	DOREPLIFETIME(ABaseBattlePawn, resistance);
	DOREPLIFETIME(ABaseBattlePawn, critical_Rate);
	DOREPLIFETIME(ABaseBattlePawn, critical_Damage);
	DOREPLIFETIME(ABaseBattlePawn, moveRange);
	DOREPLIFETIME(ABaseBattlePawn, speed);
	DOREPLIFETIME(ABaseBattlePawn, points);
	DOREPLIFETIME(ABaseBattlePawn, traits);
	DOREPLIFETIME(ABaseBattlePawn, skills);
	DOREPLIFETIME(ABaseBattlePawn, playerLifeState);
	DOREPLIFETIME(ABaseBattlePawn, lastHoveredPawn);
	DOREPLIFETIME(ABaseBattlePawn, currentTile);
	DOREPLIFETIME(ABaseBattlePawn, currentActionMode);
	DOREPLIFETIME(ABaseBattlePawn, maxHP);
	DOREPLIFETIME(ABaseBattlePawn, MyName);
	DOREPLIFETIME(ABaseBattlePawn, bIsMoving);
	DOREPLIFETIME(ABaseBattlePawn, pathArray);
	DOREPLIFETIME(ABaseBattlePawn, currentPathIndex);
	DOREPLIFETIME(ABaseBattlePawn, moveSpeed);
	DOREPLIFETIME(ABaseBattlePawn, smoothRot);
	DOREPLIFETIME(ABaseBattlePawn, newLoc);
	DOREPLIFETIME(ABaseBattlePawn, newRot);
	DOREPLIFETIME(ABaseBattlePawn, curSkillName);
	DOREPLIFETIME(ABaseBattlePawn, bWantsToAttack);
	DOREPLIFETIME(ABaseBattlePawn, bStartMontage);
	DOREPLIFETIME(ABaseBattlePawn, attackTarget);
	DOREPLIFETIME(ABaseBattlePawn, targetPlayer);
	DOREPLIFETIME(ABaseBattlePawn, maxActionPoints);
	DOREPLIFETIME(ABaseBattlePawn, curAP);

}

void ABaseBattlePawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (IsLocallyControlled())
	{
		if (this && this->currentTile)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s의 currentTile : %s"), *this->GetActorNameOrLabel(), *this->currentTile->GetActorNameOrLabel());
		}
	}
}

// Called every frame
void ABaseBattlePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Hover UI 띄우기 위한 함수
	OnMouseHover();
	
	// AStar가 끝났을 때 그 위치로 이동 나온 방향으로 이동
	// if (bIsMoving && pathArray.IsValidIndex(currentPathIndex))
	// {
	// 	// 이동 시 회전 처리
	// 	UnitRotation(this);
	// 	UnitMove(this);
	// }
	// ✅ 플레이어: 클라이언트가 소유한 경우
	if (IsLocallyControlled() && bIsMoving && pathArray.IsValidIndex(currentPathIndex))
	{
		UnitRotation(this);
		UnitMove(this); // Client → ServerRPC 호출 (OK)
	}

	// ✅ 적 유닛: 서버가 직접 처리
	if (HasAuthority() && !IsPlayerControlled() && bIsMoving && pathArray.IsValidIndex(currentPathIndex))
	{
		UnitRotation(this);
		UnitMove(this); // 서버가 직접 처리, RPC 없이 직접 SetActorLocation 사용
	}

	// 공격 시 타겟 방향으로 회전
	if (bWantsToAttack && attackTarget)
	{
		UnitAttackBeforeRoatation(this);
	}
	
	
	// UI 빌보드 처리
	BillboardBattleUnitStateUI();
	// FString str = FString::Printf(TEXT("%s,(%d,%d)"), *MyName, currentTile ? currentTile->gridCoord.X : -1, currentTile ? currentTile->gridCoord.Y : -1);
	// DrawDebugString(GetWorld(), GetActorLocation(), str, nullptr, FColor::Yellow, 0, true);
	
}

void ABaseBattlePawn::MultiCastRPC_SetBattlePlayerInfoUI_Implementation()
{
	// BattlePlayerInfo UI 세팅
	if (auto* player = Cast<ABattlePlayer>(this))
	{
		if (pc && hud && hud->mainUI && hud->mainUI->WBP_Player)
		{
			FString name = player->GetActorNameOrLabel();
			hud->mainUI->WBP_Player->SetPlayerName(name);
			hud->mainUI->WBP_Player->SetPlayerHPUI(player);
		}
	}
}

void ABaseBattlePawn::MultiCastRPC_SetEnemyName_Implementation(class ABaseEnemy* enemy)
{
	// 이름 생성 및 로그 출력
	int32 nameIndex = phaseManager->unitEnemyNameindex++;
	FString name = FString::Printf(TEXT("엔트%d"), nameIndex);
	UE_LOG(LogTemp, Warning, TEXT("Setting name to: %s"), *name);
    
	// UI가 존재하는지 확인
	if (enemy->battleUnitStateUI)
	{
		enemy->battleUnitStateUI->SetUnitName(name);
		UE_LOG(LogTemp, Warning, TEXT("Name set on UI successfully"));
	}
}

void ABaseBattlePawn::OnMouseLeftClick()
{
	if (turnManager->curUnit != this)
	{
		UE_LOG(LogTemp, Warning, TEXT("BaseBattlePawn::OnMouseLeftClick is not my pawn"));
		return;
	}
	NET_PRINTLOG(TEXT("%s OnMouseLeftClick"), *GetName());
	FVector start, end, dir;
	FHitResult hitInfo;
	FCollisionQueryParams params;

	GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(start, dir);
	end = start + dir * 100000;

	if (GetWorld()->LineTraceSingleByChannel(hitInfo, start, end,ECC_Visibility, params))
	{
		if (AGridTile* clickedTile = Cast<AGridTile>(hitInfo.GetActor()))
		{
			if (!highlightedTiles.Contains(clickedTile))
			{
				UE_LOG(LogTemp, Warning, TEXT("클릭한 타일이 이동 범위에 없습니다."));
				return;
			}
		}
		
		EActionMode curSkillState = currentActionMode;
		UE_LOG(LogTemp, Warning, TEXT("Test Click : Current Action Mode %s"),*UEnum::GetValueAsString(curSkillState));

		// UI에 띄울 스킬 이름 저장 
		// SkillNameJudgment(curSkillState);
		Server_SkillName(curSkillState);
		
		switch (curSkillState)
		{
		case EActionMode::None:
			break;
		case EActionMode::TurnEnd:
			if (HasAuthority())
			{
				turnManager->OnTurnEnd();
			}
			break;
		case EActionMode::Move:
			PlayerMove(hitInfo);
			break;
		case EActionMode::BaseAttack:
			PlayerBaseAttack(hitInfo);
			UE_LOG(LogTemp, Warning, TEXT("OnMouseLeftClick BaseAttack"));
			break;
		case EActionMode::Poison:
			PlayerPoison(hitInfo);
			break;
		case EActionMode::Fatal:
			PlayerFatal(hitInfo);
			break;
		case EActionMode::Rupture:
			PlayerRupture(hitInfo);
			break;
		}
	}
}

// void ABaseBattlePawn::OnRep_ActionModeChanged()
// {
// 	if (auto* player = Cast<ABattlePlayer>(this))
// 	{
// 		if (player->playerAnim) player->playerAnim->actionMode = currentActionMode;
// 	}
// 	else if (auto* enemy = Cast<ABaseEnemy>(this))
// 	{
// 		if (enemy->enemyAnim) enemy->enemyAnim->actionMode = currentActionMode;
// 	}
// }

void ABaseBattlePawn::PlayerMove(FHitResult& hitInfo)
{
	// 이전 그리드 타일 클리어
	ClearGridTile();
	// AStar로 이동
	if (AGridTile* testTile = Cast<AGridTile>(hitInfo.GetActor()))
	{
		if (currentTile == testTile)
		{
			UE_LOG(LogTemp, Warning, TEXT("CurrentTile == TestTile"));
			return;
		}

		// AnimInstance actionMode 업데이트
		if (IsLocallyControlled())
		{
			if (auto* player = Cast<ABattlePlayer>(this)) player->Server_UpdatePlayerAnim(player->currentActionMode);
		}
		else if (HasAuthority())
		{
			if (auto* player = Cast<ABattlePlayer>(this)) player->MultiCastRPC_UpdatePlayerAnim(player->currentActionMode);
		}

		
		// 타일 초기화
		InitValues();

		goalTile = testTile;

		// 시작 타일 찾기
		FHitResult startHit;
		FVector start = GetActorLocation();
		FVector end = start + FVector::DownVector * 1000;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(startHit, start, end, ECC_Visibility, params))
		{
			startTile = Cast<AGridTile>(startHit.GetActor());
			if (startTile)
			{
				startTile->sCostValue = 0;
				openArray.Add(startTile);

				// A* 시작
				PathFind();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("StartTile is nullptr"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to trace start tile"));
		}
	}
}

void ABaseBattlePawn::Server_UpdateCurrentTile_Implementation(class AGridTile* tile)
{
	currentTile = tile;
}


void ABaseBattlePawn::PlayerBaseAttack(FHitResult& hitInfo)
{
	if (!bBaseAttack)
	{
		UE_LOG(LogTemp, Warning, TEXT("Use BaseAttack Before"));
		return;
	}

	bBaseAttack = false;

	AActor* hitActor = hitInfo.GetActor();

	ABattlePlayer* thisAsPlayer = Cast<ABattlePlayer>(this);
	ABaseEnemy*   thisAsEnemy  = Cast<ABaseEnemy>(this);

	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitActor))
	{
		targetEnemy = enemy;
		attackTarget = targetEnemy;
		bWantsToAttack = true;

		if (thisAsPlayer)
		{
			if (IsLocallyControlled()) thisAsPlayer->Server_UpdatePlayerAnim(thisAsPlayer->currentActionMode);
			else if (HasAuthority()) thisAsPlayer->MultiCastRPC_UpdatePlayerAnim(thisAsPlayer->currentActionMode);
		}
	}
	// 공격 대상이 player라면
	// else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitActor))
	// {
	// 	targetPlayer = player;
	// 	attackTarget = targetPlayer;
	// 	bWantsToAttack = true;
	// 	NET_PRINTLOG(TEXT("targetPlayer = %s"), *targetPlayer->GetActorNameOrLabel());
	// 	if (thisAsEnemy)
	// 	{
	// 		thisAsEnemy->ServerRPC_UpdateEnemyAnim(thisAsEnemy->currentActionMode);
	// 		thisAsEnemy->bStartMontage = true;
	// 	}
	// }
}


void ABaseBattlePawn::PlayerPoison(FHitResult& hitInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerPoison In"));
	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		// if (enemy->enemyAnim)
		// {
		// 	enemy->enemyAnim->actionMode = currentActionMode;
		// }
		// enemy->ServerRPC_UpdateEnemyAnim(enemy->currentActionMode);
		if (ABattlePlayer* player = Cast<ABattlePlayer>(this))
		{
			targetEnemy = enemy;
			attackTarget = targetEnemy;
			bWantsToAttack = true;
			
			if (IsLocallyControlled()) player->Server_UpdatePlayerAnim(player->currentActionMode);
			else if (HasAuthority()) player->MultiCastRPC_UpdatePlayerAnim(player->currentActionMode);
		}
	}
}

void ABaseBattlePawn::PlayerFatal(FHitResult& hitInfo)
{
	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		// if (enemy->enemyAnim)
		// {
		// 	enemy->enemyAnim->actionMode = currentActionMode;
		// }
		if (ABattlePlayer* player = Cast<ABattlePlayer>(this))
		{
			targetEnemy = enemy;
			attackTarget = targetEnemy;
			bWantsToAttack = true;
			
			if (IsLocallyControlled()) player->Server_UpdatePlayerAnim(player->currentActionMode);
			else if (HasAuthority()) player->MultiCastRPC_UpdatePlayerAnim(player->currentActionMode);
		}
	}
}

void ABaseBattlePawn::PlayerRupture(FHitResult& hitInfo)
{
	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		// if (enemy->enemyAnim)
		// {
		// 	enemy->enemyAnim->actionMode = currentActionMode;
		// }
		if (ABattlePlayer* player = Cast<ABattlePlayer>(this))
		{
			targetEnemy = enemy;
			attackTarget = targetEnemy;
			bWantsToAttack = true;
			
			if (IsLocallyControlled()) player->Server_UpdatePlayerAnim(player->currentActionMode);
			else if (HasAuthority()) player->MultiCastRPC_UpdatePlayerAnim(player->currentActionMode);
		}
	}
}

void ABaseBattlePawn::GetDamage(ABaseBattlePawn* unit, int32 damage)
{
	// player HP가 0보다 크다면
	UE_LOG(LogTemp, Warning, TEXT("In GetDamage"));
	if (ABattlePlayer* player = Cast<ABattlePlayer>(unit))
	{
		if (!battleUnitStateUI)
		{
			UE_LOG(LogTemp, Warning,TEXT("GetDamage(Player) : !battleUnitStateUI"));
			return;
		}
		if (!(hud && hud->mainUI && hud->mainUI->WBP_Player))
		{
			UE_LOG(LogTemp, Warning,TEXT("GetDamage(Player) : !(hud && hud->mainUI && hud->mainUI->WBP_Player)"));
			return;
		}
		if (!phaseManager)
		{
			UE_LOG(LogTemp, Warning,TEXT("GetDamage(Player) : !phaseManager"));
			return;
		}
		if (!player->playerAnim)
		{
			UE_LOG(LogTemp, Warning,TEXT("GetDamage(Player) : !player->playerAnim"));
			return;
		}
		MultiCastRPC_PlayerGetDamage(player, damage);
	}
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(unit))
	{
		if (!battleUnitStateUI)
		{
			UE_LOG(LogTemp, Warning,TEXT("GetDamage(Enemy) : !battleUnitStateUI"));
			return;
		}
		if (!(hud && hud->mainUI && hud->mainUI->WBP_Player))
		{
			UE_LOG(LogTemp, Warning,TEXT("GetDamage(Enemy) : !(hud && hud->mainUI && hud->mainUI->WBP_Player)"));
			return;
		}
		if (!phaseManager)
		{
			UE_LOG(LogTemp, Warning,TEXT("GetDamage(Enemy) : !phaseManager"));
			return;
		}
		if (!enemy->enemyAnim)
		{
			UE_LOG(LogTemp, Warning,TEXT("GetDamage(Enemy) : !Enemy->playerAnim"));
			return;
		}
		MultiCastRPC_EnemyGetDamage(enemy, damage);
	}
}

void ABaseBattlePawn::MultiCastRPC_EnemyGetDamage_Implementation(class ABaseEnemy* enemy, int32 damage)
{
	if (!IsValid(enemy) || enemy->IsPendingKillPending()) return;
	// 피를 깎는다.
	enemy->hp = FMath::Max(0, enemy->hp - damage);
	UE_LOG(LogTemp, Warning, TEXT("Damage : %d, enemyHP : %d"), damage, enemy->hp);
	// UI HP 업데이트
	enemy->battleUnitStateUI->UpdateHP(enemy->hp, enemy);

	// 공격 몽타주 실행
	enemy->enemyAnim->PlayHitMotionAnimation(TEXT("StartHitMotion"));
	UE_LOG(LogTemp, Warning, TEXT("anim Set! Hit Animation !!"));

	// Actor 위치 옮기고 UI Actor 보이게
	FVector loc = FVector(enemy->GetActorLocation().X, enemy->GetActorLocation().Y,enemy->GetActorLocation().Z + 250);
	phaseManager->damageUIActor->SetActorLocation(loc);
	phaseManager->damageUIActor->ShowDamageUI();
	// 대미지 텍스트 변경
	phaseManager->damageUIActor->damageUI->SetDamageText(damage);

	// hp가 0보다 작으면 사망
	if (enemy->hp <= 0)
	{
		// enemy 자체에 State 세팅
		enemy->enemyLifeState = ELifeState::Dead;
		// enemy Anim에 State 세팅
		enemy->enemyAnim->lifeState = ELifeState::Dead;
		UE_LOG(LogTemp, Warning, TEXT("Enemy Dead %s"),*UEnum::GetValueAsString(enemy->enemyLifeState));
	}
	// 대미지를 줬다면 현재 공격 상태를 None으로 초기화
	if (auto* anim = Cast<UBattleEnemyAnimInstance>(enemy->meshComp->GetAnimInstance()))
	{
		anim->actionMode = currentActionMode;
	}
}

void ABaseBattlePawn::MultiCastRPC_PlayerGetDamage_Implementation(class ABattlePlayer* player, int32 damage)
{
	if (!IsValid(player) || player->IsPendingKillPending()) return;
	
	// 피를 깎는다.
	player->hp = FMath::Max(0, player->hp - damage);
	UE_LOG(LogTemp, Warning, TEXT("Damage : %d, playerHP : %d"), damage, player->hp);
		
	// HP를 UI에 업데이트
	// BattlePlayerInfo HP 업데이트
	player->battleUnitStateUI->UpdateHP(player->hp, player);
	player->hud->mainUI->WBP_Player->PlayerUpdateHP(player, player->hp);

	player->playerAnim->PlayHitMotionAnimation(TEXT("StartPlayerHitMotion"));
	UE_LOG(LogTemp, Warning, TEXT("anim Set! Hit Animation !!"));

	// Actor 위치 옮기고 UI Actor 보이게
	FVector loc = FVector(player->GetActorLocation().X,player->GetActorLocation().Y,player->GetActorLocation().Z + 250);
	phaseManager->damageUIActor->SetActorLocation(loc);
	phaseManager->damageUIActor->ShowDamageUI();
	// 대미지 텍스트 변경
	phaseManager->damageUIActor->damageUI->SetDamageText(damage);

	// hp가 0보다 작으면 사망
	if (player->hp <= 0)
	{
		// player에 세팅
		player->playerLifeState = ELifeState::Dead;
		// playerAnim에 세팅
		player->playerAnim->lifeState = ELifeState::Dead;
		UE_LOG(LogTemp, Warning, TEXT("Enemy Dead %s"), *UEnum::GetValueAsString(player->playerLifeState));
	}
}

void ABaseBattlePawn::Server_PlayerApplyAttack_Implementation(ABaseBattlePawn* targetUnit, EActionMode attackType)
{
	PlayerApplyAttack_Implementation(targetUnit, attackType);
}

void ABaseBattlePawn::PlayerApplyAttack_Implementation(ABaseBattlePawn* targetUnit, EActionMode attackType)
{
	auto* player = Cast<ABattlePlayer>(this);
	auto* enemy = Cast<ABaseEnemy>(targetUnit);
	if (!player || !enemy)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Player Attack Enemy"));
	int32 atk = player->attack;
	int32 weapon = 0;
	float critical = player->critical_Rate;
	float criticalDamage = player->critical_Damage;
	int32 personality = 0;
	int32 status_effect = 0;
	int32 damage = 0;

	// 스킬 계수 및 추가 효과
	float skillMultiplier = 1.0f;

	switch (attackType)
	{
	case EActionMode::Poison:
		if (!player->CanConsumeAP(1))
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
			return;
		}
	
		player->AddStatusEffect(EStatusEffect::Poison, 3);
		break;
	case EActionMode::Fatal:
		if (!player->CanConsumeAP(2))
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
			return;
		}
		skillMultiplier = 1.5f;
		player->AddStatusEffect(EStatusEffect::Bleeding, 2);
		break;
	case EActionMode::Rupture:
		if (!player->CanConsumeAP(2))
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
			return;
		}
		skillMultiplier = 1.5f;
		player->AddStatusEffect(EStatusEffect::Vulnerable, 1);
		break;
	default:
		// 기본 타격 스킬 배율
		skillMultiplier = 1.0f;
		break;
	}

	// AP 감소 UI에 반영
	InitAPUI();
	
	CalculateAndApplyDamage(enemy, atk, skillMultiplier, critical, criticalDamage);

	// 대미지를 줬다면 현재 공격 상태를 None으로 초기화
	currentActionMode = EActionMode::None;
	if (player->playerAnim) player->playerAnim->actionMode = currentActionMode;
}
void ABaseBattlePawn::EnemyApplyAttack(ABaseBattlePawn* targetUnit, EActionMode attackType)
{
	auto* player = Cast<ABattlePlayer>(targetUnit);
	auto* enemy = Cast<ABaseEnemy>(this);
	if (!player || !enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("!player || !enemy"));
		return;
	}

	UE_LOG(LogTemp, Warning,TEXT("[EnemyApplyAttack] Enemy %s attacking Player %s"),*enemy->GetName(), *player->GetName());

	int32 atk = enemy->attack;
	float critical = enemy->critical_Rate;
	float criticalDamage = enemy->critical_Damage;
	float skillMultiplier = 1.0f;

	switch (attackType)
	{
	case EActionMode::Paralysis:
		if (!enemy->TryConsumeAP(1))
		{
			return;
		}
		skillMultiplier = 0.8f;
		enemy->AddStatusEffect(EStatusEffect::Weakening, 2);
		break;

	case EActionMode::Poison:
		if (!enemy->TryConsumeAP(1))
		{
			return;
		}
		UE_LOG(LogTemp, Warning, TEXT("In Poison"));
		enemy->AddStatusEffect(EStatusEffect::Poison, 3);
		break;

	case EActionMode::Vulnerable:
		if (!enemy->TryConsumeAP(1))
		{
			return;
		}
		enemy->AddStatusEffect(EStatusEffect::Vulnerable, 2);
		break;

	case EActionMode::Weakening:
		if (!enemy->TryConsumeAP(1))
		{
			return;
		}
		enemy->AddStatusEffect(EStatusEffect::Weakening, 2);
		break;

	case EActionMode::Fatal:
		if (!enemy->TryConsumeAP(2))
		{
			return;
		}
		skillMultiplier = 1.5f;
		enemy->AddStatusEffect(EStatusEffect::Bleeding, 2);
		break;

	case EActionMode::Rupture:
		if (!enemy->TryConsumeAP(2))
		{
			return;
		}
		skillMultiplier = 1.5f;
		enemy->AddStatusEffect(EStatusEffect::Vulnerable, 1);
		break;

	case EActionMode::Roar:
		if (!enemy->TryConsumeAP(1))
		{
			return;
		}
		enemy->AddStatusEffect(EStatusEffect::Weakening, 2);
		return;

	case EActionMode::BattleCry:
		if (!enemy->TryConsumeAP(1))
		{
			return;
		}
		enemy->AddStatusEffect(EStatusEffect::Angry, 1);
		return;

	default:
		break;
	}

	// 대미지 처리
	CalculateAndApplyDamage(player, atk, skillMultiplier, critical, criticalDamage);
	
	// 대미지를 줬다면 None으로 변경
	currentActionMode = EActionMode::None;
	if (enemy->enemyAnim) enemy->enemyAnim->actionMode = currentActionMode;
}

void ABaseBattlePawn::CalculateAndApplyDamage(ABaseBattlePawn* target, int32 atk, float skillMultiplier, float criticalRate, float criticalDamage)
{
	UE_LOG(LogTemp, Warning, TEXT("In Calculate"));
	int32 damage = 0;
	int32 chance = FMath::RoundToInt(criticalRate * 100.0f);
	int32 roll = FMath::RandRange(1, 100);

	if (bool bIsCrit = roll <= chance)
	{
		damage = (atk * skillMultiplier) * criticalDamage;
		UE_LOG(LogTemp, Warning, TEXT("Critical Damage : %d"), damage);
	}
	else
	{
		damage = atk * skillMultiplier;
		UE_LOG(LogTemp, Warning, TEXT("Damage : %d"), damage);
	}

	GetDamage(target, damage);
}

void ABaseBattlePawn::AddStatusEffect(EStatusEffect newEffect, int32 duration)
{
	if (activeStatusEffects.Contains(newEffect))
	{
		// 이미 있으면 지속시간 갱신 or 덧붙이기
		activeStatusEffects[newEffect] = FMath::Max(activeStatusEffects[newEffect], duration);
		// UEnum로그를 찍기 위해 enum 메타데이터를 얻어서 Log찍는 과정 
		UEnum* enumPtr = StaticEnum<EStatusEffect>();
		if (enumPtr) UE_LOG(LogTemp, Warning,TEXT("In AddStatusEffect : %s, Duration : %d"),*enumPtr->GetNameStringByValue(static_cast<int64>(newEffect)), activeStatusEffects[newEffect]);
	}
	else
	{
		activeStatusEffects.Add(newEffect, duration);
	}
}

// 턴 시작 시 자신이 가진 상태이상 효과들을 체크해서 대미지, 버프, 디버프 부여  
void ABaseBattlePawn::ApplyStatusEffect()
{
	if (activeStatusEffects.Num() < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Status Effect"));
		return;
	}

	TArray<EStatusEffect> expiredEffects;

	for (auto& elem : activeStatusEffects)
	{
		// Key로 된 Enum값 주소 저장
		EStatusEffect effect = elem.Key;
		// element값의 주소 저장
		int32& TurnsLeft = elem.Value;

		// switch문 처리용 함수
		HandleStatusEffect(effect);

		// 턴 감소 처리
		TurnsLeft--;
		if (TurnsLeft <= 0)
		{
			expiredEffects.Add(effect);
		}
	}

	// 만료된 효과 제거
	for (EStatusEffect effect : expiredEffects)
	{
		activeStatusEffects.Remove(effect);
	}
}

void ABaseBattlePawn::HandleStatusEffect(EStatusEffect effect)
{
	if (!(battleUnitStateUI && pc && hud && hud->mainUI && hud->mainUI->
		WBP_Player))
	{
		UE_LOG(LogTemp, Warning,TEXT("HandleStatus : !battleUnitStateUI && pc && hud && hud->mainUI && hud->mainUI->WBP_Player"));
		return;
	}
	switch (effect)
	{
	case EStatusEffect::Poison:
		if (auto* player = Cast<ABattlePlayer>(this))
		{
			player->hp -= 5;
			battleUnitStateUI->UpdateHP(player->hp, player);
			// BattlePlayerInfo UI 세팅
			hud->mainUI->WBP_Player->SetPlayerHPUI(player);
		}
		else if (auto* enemy = Cast<ABaseEnemy>(this))
		{
			enemy->hp -= 5;
			battleUnitStateUI->UpdateHP(enemy->hp, enemy);
		}
		break;
	case EStatusEffect::Vulnerable:
		if (auto* player = Cast<ABattlePlayer>(this))
		{
			VulnerableProcess();
		}
		else if (auto* enemy = Cast<ABaseEnemy>(this))
		{
			VulnerableEnemyProcess(enemy);
		}
		break;
	case EStatusEffect::Weakening:
		if (auto* player = Cast<ABattlePlayer>(this))
		{
			WeakeningProcess();
		}
		else if (auto* enemy = Cast<ABaseEnemy>(this))
		{
			WeakeningEnemyProcess(enemy);
		}
		break;
	case EStatusEffect::Angry:
		if (auto* player = Cast<ABattlePlayer>(this))
		{
			AngryProcess();
		}
		else if (auto* enemy = Cast<ABaseEnemy>(this))
		{
			AngryEnemyProcess(enemy);
		}
		break;
	case EStatusEffect::Bleeding:
		if (auto* player = Cast<ABattlePlayer>(this))
		{
			player->hp -= 5;
			battleUnitStateUI->UpdateHP(player->hp, player);

			// BattlePlayerInfo UI 세팅
			hud->mainUI->WBP_Player->SetPlayerHPUI(player);
			// 출혈로 인한 스텟 상태 변경 로직 
			BleedingProcess();
		}
		else if (auto* enemy = Cast<ABaseEnemy>(this))
		{
			enemy->hp -= 5;
			battleUnitStateUI->UpdateHP(enemy->hp, enemy);
			BleedingEnemyProcess(enemy);
		}
		break;
	default:
		break;
	}
}

void ABaseBattlePawn::WeakeningProcess()
{
	// 약화 상태이상 처리 프로세스
	// 공격력과 방어력 스탯 감소
	int32& atk = attack;
	int32& def = defense;
	// 반올림 처리
	int32 decreaseAtkAmount = FMath::RoundToInt(atk * 0.2f);
	int32 decreaseDefAmount = FMath::RoundToInt(def * 0.2f);
	// 음수 방지
	atk = FMath::Max(0, atk - decreaseAtkAmount);
	def = FMath::Max(0, def - decreaseDefAmount);
	UE_LOG(LogTemp, Warning, TEXT("WeakeningProcess : %d"), atk);
	UE_LOG(LogTemp, Warning, TEXT("WeakeningProcess : %d"), def);
}

void ABaseBattlePawn::VulnerableProcess()
{
	// 방어력 감소 처리
	int32& def = defense;
	int32 decreaseDefAmount = FMath::RoundToInt(def * 0.5f);
	def = FMath::Max(0, def - decreaseDefAmount);
	UE_LOG(LogTemp, Warning, TEXT("VulnerableProcess : %d"), def);
}

void ABaseBattlePawn::AngryProcess()
{
	// 공격력 증가 처리
	int32& atk = attack;
	int32 decreaseAtkAmount = FMath::RoundToInt(atk * 0.5f);
	atk = FMath::Max(0, atk - decreaseAtkAmount);
	UE_LOG(LogTemp, Warning, TEXT("WeakeningProcess : %d"), atk);
}

void ABaseBattlePawn::BleedingProcess()
{
	// 방어력 감소 처리
	int32& def = defense;
	int32 decreaseDefAmount = FMath::RoundToInt(def * 0.2f);
	def = FMath::Max(0, def - decreaseDefAmount);
	UE_LOG(LogTemp, Warning, TEXT("VulnerableProcess : %d"), def);
}

void ABaseBattlePawn::WeakeningEnemyProcess(class ABaseEnemy* enemy)
{
	// 약화 상태이상 처리 프로세스
	// 공격력과 방어력 스탯 감소
	int32& atk = enemy->attack;
	int32& def = enemy->defense;
	// 반올림 처리
	int32 decreaseAtkAmount = FMath::RoundToInt(atk * 0.2f);
	int32 decreaseDefAmount = FMath::RoundToInt(def * 0.2f);
	// 음수 방지
	atk = FMath::Max(0, atk - decreaseAtkAmount);
	def = FMath::Max(0, def - decreaseDefAmount);
	UE_LOG(LogTemp, Warning, TEXT("WeakeningProcess : %d"), atk);
	UE_LOG(LogTemp, Warning, TEXT("WeakeningProcess : %d"), def);
}

void ABaseBattlePawn::VulnerableEnemyProcess(class ABaseEnemy* enemy)
{
	// 방어력 감소 처리
	int32& def = enemy->defense;
	int32 decreaseDefAmount = FMath::RoundToInt(def * 0.5f);
	def = FMath::Max(0, def - decreaseDefAmount);
	UE_LOG(LogTemp, Warning, TEXT("VulnerableProcess : %d"), def);
}

void ABaseBattlePawn::AngryEnemyProcess(class ABaseEnemy* enemy)
{
	// 공격력 증가 처리
	int32& atk = enemy->attack;
	int32 decreaseAtkAmount = FMath::RoundToInt(atk * 0.5f);
	atk = FMath::Max(0, atk - decreaseAtkAmount);
	UE_LOG(LogTemp, Warning, TEXT("WeakeningProcess : %d"), atk);
}

void ABaseBattlePawn::BleedingEnemyProcess(class ABaseEnemy* enemy)
{
	// 방어력 감소 처리
	int32& def = enemy->defense;
	int32 decreaseDefAmount = FMath::RoundToInt(def * 0.2f);
	def = FMath::Max(0, def - decreaseDefAmount);
	UE_LOG(LogTemp, Warning, TEXT("VulnerableProcess : %d"), def);
}

void ABaseBattlePawn::PathFind()
{
	if (openArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenArray is empty"));
		return;
	}

	int32 safetyCounter = 0;
	constexpr int32 maxSafetyCount = 1000;

	while (openArray.Num() > 0 && safetyCounter++ < maxSafetyCount)
	{
		UE_LOG(LogTemp, Warning,TEXT("BuildPath: pathArray.Num = %d, moveRange = %d"),pathArray.Num(), moveRange);
		UE_LOG(LogTemp, Warning, TEXT("Final bIsMoving = %s"), bIsMoving ? TEXT("true") : TEXT("false"));
		if (safetyCounter > maxSafetyCount)
		{
			UE_LOG(LogTemp, Warning, TEXT("Path Safe Break"));
			break;
		}
		currentTile = openArray[0];
		openArray.RemoveAt(0);

		UE_LOG(LogTemp, Warning, TEXT("currentTile = %s, Coord = (%d, %d)"),*currentTile->GetName(),gridTileManager->GetTileLoc(currentTile).X,gridTileManager->GetTileLoc(currentTile).Y);
		if (!IsValid(currentTile))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s currentTile PathFind Error"),*GetName());
			turnManager->OnTurnEnd();
			return;
		}
		if (!IsValid(goalTile))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s goalTile PathFind Error"),*GetName());
			turnManager->OnTurnEnd();
			return;
		}
		// 목표 도달했으면 종료
		if (currentTile == goalTile)
		{
			// 로컬이라면 서버로 currentTile 업데이트
			if (IsLocallyControlled()) Server_UpdateCurrentTile(currentTile);
			BuildPath();
			return;
		}

		if (IsValid(currentTile))
		{
			AddOpenByOffset({1, 0});
			AddOpenByOffset({-1, 0});
			AddOpenByOffset({0, 1});
			AddOpenByOffset({0, -1});

			closedArray.Add(currentTile);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("길을 찾지 못했습니다"));
	turnManager->OnTurnEnd();
}

void ABaseBattlePawn::AddOpenByOffset(FIntPoint offset)
{
	FIntPoint nextCoord = currentTile->gridCoord + offset;
	
	if (AGridTile* tile = gridTileManager->map.FindRef(nextCoord))
	{
		float previousCost = tile->tCostValue;
		tile->SetCost(currentTile, goalTile);
		
		if (tile != currentTile && (!tile->parentTile || tile->tCostValue < previousCost))
		{
			tile->parentTile = currentTile;

			int32 i = 0;
			for (; i < openArray.Num(); ++i)
			{
				if (openArray[i]->tCostValue >= tile->tCostValue)
				{
					break;
				}
			}
			openArray.Insert(tile, i);
		}
	}
}

void ABaseBattlePawn::BuildPath()
{
	UE_LOG(LogTemp, Warning, TEXT("BuildPath: moveRange = %d"), moveRange);

	if (auto* player = Cast<ABattlePlayer>(this))
	{
		// goalTile 또는 goalTile->parentTile 자체가 nullptr일 수 있다.
		if (!goalTile || !goalTile->parentTile)
		{
			UE_LOG(LogTemp, Error,TEXT("BuildPath aborted: goalTile or parent is null"));
			turnManager->OnTurnEnd();
			return;
		}
		if (startTile == goalTile)
		{
			UE_LOG(LogTemp, Warning, TEXT("Start tile is same as goal tile"));
			turnManager->OnTurnEnd();
			return;
		}

		TSet<AGridTile*> visitePathTiles;
		// 찾은 길 표시
		AGridTile* temp = goalTile;

		while (temp && temp->parentTile)
		{
			if (visitePathTiles.Contains(temp) || temp == temp->parentTile)
			{
				UE_LOG(LogTemp, Warning,TEXT(" Infinite loop detected in BuildPath"));
				break;
			}

			visitePathTiles.Add(temp);
			pathArray.Insert(temp, 0); // 역방향으로 삽입
			temp = temp->parentTile;
		}

		if (pathArray.Num() == 0)
		{
			turnManager->OnTurnEnd(); // 이동 실패 시 턴 종료
			return;
		}
		
		// 경로 저장 완료했으면 이동 시작
		if (pathArray.Num() > moveRange)
		{
			UE_LOG(LogTemp, Warning,TEXT("Path too long! moveRange = %d, path length = %d"), moveRange, pathArray.Num());
			// 최대 이동 가능 거리만큼 잘라 이동
			pathArray.SetNum(moveRange); 
		}
	}
	else if (auto* enemy = Cast<ABaseEnemy>(this))
	{
		// goalTile 또는 goalTile->parentTile 자체가 nullptr일 수 있다.
		if (!goalTile || !goalTile->parentTile)
		{
			UE_LOG(LogTemp, Error,TEXT("BuildPath aborted: goalTile or parent is null"));
			turnManager->OnTurnEnd();
			return;
		}
		if (startTile == goalTile)
		{
			UE_LOG(LogTemp, Warning, TEXT("Start tile is same as goal tile"));
			turnManager->OnTurnEnd();
			return;
		}

		TSet<AGridTile*> visitePathTiles;
		// 찾은 길 표시
		AGridTile* temp = goalTile;

		while (temp && temp->parentTile)
		{
			if (visitePathTiles.Contains(temp) || temp == temp->parentTile)
			{
				UE_LOG(LogTemp, Warning,TEXT(" Infinite loop detected in BuildPath"));
				break;
			}

			visitePathTiles.Add(temp);
			pathArray.Insert(temp, 0); // 역방향으로 삽입
			temp = temp->parentTile;
		}
	
		bool bCanReachGoalTile = pathArray.Last() == goalTile && pathArray.Num() <= moveRange;
		if (bCanReachGoalTile)
		{
			UE_LOG(LogTemp, Warning, TEXT("goalTile 직전까지 이동"));
			pathArray.RemoveAt(pathArray.Num() - 1); // goalTile 제거
		}

		// pathArray에 개수가 moveRange보다 크면 mvoeRange만큼만 잘라서 이동
		if (pathArray.Num() > moveRange)
		{
			UE_LOG(LogTemp, Warning,TEXT("Path too long! moveRange = %d, path length = %d"), moveRange, pathArray.Num());
			// 최대 이동 가능 거리만큼 잘라 이동
			pathArray.SetNum(moveRange); 
		}

		if (pathArray.Num() == 0)
		{
			bIsMoving = false;
			enemy->FindAndAttackPlayer();
			return;
		}
		
		// enemy actionmode 업데이트
		enemy->currentActionMode = EActionMode::Move;
		enemy->MultiCastRPC_UpdateEnemyAnim(currentActionMode);
		UE_LOG(LogTemp, Warning,TEXT("Processing anim actionMode Update !! %s"), *UEnum::GetValueAsString(enemy->enemyAnim->actionMode));
	}
	
	// InitValues();
	ClearGridTile();
	bIsMoving = true;
	currentPathIndex = 0;
}

void ABaseBattlePawn::AddOpenArray(FVector dir)
{
	FVector start = currentTile->GetActorLocation();
	FVector end = start + dir * 200;

	FHitResult hitInfo;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	params.AddIgnoredActor(currentTile);

	DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 5.0f);

	if (bool isHit = GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Visibility, params))
	{
		AGridTile* tile = Cast<AGridTile>(hitInfo.GetActor());
		// openArray, closeArray에 해당 tile이 없을 때만
		if (IsValid(tile) && !openArray.Contains(tile) && !closedArray.Contains(tile))
		{
			// 해당 tile에 Cost 구하기
			tile->SetCost(currentTile, goalTile);

			// openArray 값에 tile 추가 (cost 값이 작은 tile 앞쪽에)
			int32 i = 0;
			for (i = 0; i < openArray.Num(); ++i)
			{
				if (openArray[i]->tCostValue >= tile->tCostValue) break;
			}
			openArray.Insert(tile, i);
		}
	}
}

void ABaseBattlePawn::Multicast_SeeMoveRange_Implementation()
{
	TArray<FIntPoint> tiles;

	// 서버에서만 로직 수행
	SeeMoveRange(moveRange, tiles);

	highlightedTiles.Empty();
	
	for (const FIntPoint& coord : tiles)
	{
		AGridTile* tile = gridTileManager->GetTile(coord.X, coord.Y);
		if (tile)
		{
			gridTileManager->SetTileColor(tile, true);
			highlightedTiles.Add(tile);
		}
	}
}

void ABaseBattlePawn::SeeMoveRange(int32 move_Range, TArray<FIntPoint>& tiles)
{
	tiles.Empty();
	highlightedTiles.Empty();
	
	FIntPoint coord = gridTileManager->GetTileLoc(currentTile);
	int32 startX = coord.X;
	int32 startY = coord.Y;

	for (int y = startY - move_Range; y <= startY + move_Range; y++)
	{
		for (int x = startX - move_Range; x <= startX + move_Range; x++)
		{
			int dx = FMath::Abs(x - startX);
			int dy = FMath::Abs(y - startY);

			if (dx + dy <= move_Range)
			{
				AGridTile* tile = gridTileManager->GetTile(x, y);
				if (tile)
				{
					tiles.Add(FIntPoint(x, y));
					highlightedTiles.Add(tile); // 서버용 저장
				}
			}
		}
	}
}
void ABaseBattlePawn::ClearGridTile()
{
	for (AGridTile* target : highlightedTiles)
	{
		if (target)
		{
			gridTileManager->SetTileColor(target, false);
		}
	}
}

void ABaseBattlePawn::OnRep_TargetLoc()
{
	bIsMoving = true;
}

void ABaseBattlePawn::MultiCastRPC_UpdateNewLoc_Implementation(const FVector& newLocation)
{
	SetActorLocation(newLocation);
}

void ABaseBattlePawn::InitValues()
{
	if (!gridTileManager)
	{
		return;
	}

	for (auto& pair : gridTileManager->map)
	{
		if (pair.Value)
		{
			pair.Value->parentTile = nullptr;
			pair.Value->sCostValue = TNumericLimits<float>::Max();
			pair.Value->tCostValue = TNumericLimits<float>::Max();
		}
	}
	for (AGridTile* tile : closedArray)
	{
		if (IsValid(tile))
		{
			tile->parentTile = nullptr;
		}
	}

	for (AGridTile* tile : openArray)
	{
		if (IsValid(tile))
		{
			tile->parentTile = nullptr;
		}
	}

	// 배열 및 변수 초기화
	openArray.Empty();
	closedArray.Empty();
	pathArray.Empty();
	currentPathIndex = 0;
	bIsMoving = false;
	// currentTile = nullptr;
	startTile = nullptr;
	goalTile = nullptr;
}


void ABaseBattlePawn::BillboardBattleUnitStateUI()
{
	if (!battleUnitStateUI) return;
	if (!phaseManager->damageUIActor)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("BillboardBattleUnitStateUI : !phaseManager->damageUIActor"
		       ));
		return;
	}
	// 내가 컨트롤하고 있는 카메라를 가져오자
	AActor* cam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	// 카메라의 앞방향(반대), 윗방향을 이용해서 Rotator를 구하자.
	FRotator rot = UKismetMathLibrary::MakeRotFromXZ(
		-cam->GetActorForwardVector(), cam->GetActorUpVector());
	// battleUnitStateComp를 구한 Rotator 값으로 설정.
	battleUnitStateComp->SetWorldRotation(rot);

	// Actor 위치 옮기고 UI Actor 보이게
	phaseManager->damageUIActor->damageUIComp->SetWorldRotation(rot);
}

void ABaseBattlePawn::OnMouseHover()
{
	if (!battleUnitStateUI)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnMouseHover : !battleUnitStateUI"));
		return;
	}
	FVector start, end, dir;
	FHitResult hitInfo;
	FCollisionQueryParams params;

	GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(start, dir);
	end = start + dir * 10000;

	ABaseBattlePawn* currentHovered = nullptr;

	if (GetWorld()->LineTraceSingleByChannel(hitInfo, start, end,ECC_Visibility, params))
	{
		currentHovered = Cast<ABaseBattlePawn>(hitInfo.GetActor());
	}

	// 이전과 현재가 다르면 UI 갱신
	if (lastHoveredPawn != currentHovered)
	{
		if (lastHoveredPawn && lastHoveredPawn->battleUnitStateUI)
		{
			lastHoveredPawn->battleUnitStateUI->ShowBaseUI();
			lastHoveredPawn->battleUnitStateComp->SetDrawSize(FVector2D(50, 10));
		}

		if (currentHovered && currentHovered->battleUnitStateUI)
		{
			currentHovered->battleUnitStateUI->SetUnitName(currentHovered->GetActorNameOrLabel());
			currentHovered->battleUnitStateUI->UpdatePlayerHPUI(currentHovered->hp);
			
			currentHovered->battleUnitStateUI->ShowHoverUI();
			currentHovered->battleUnitStateComp->SetDrawSize(FVector2D(150, 130));
		}

		lastHoveredPawn = currentHovered;
	}
}

void ABaseBattlePawn::Server_SkillName_Implementation(const EActionMode curAction)
{
	MultiCast_SkillName(curAction);
}

void ABaseBattlePawn::MultiCast_SkillName_Implementation(const EActionMode curAction)
{
	SkillNameJudgment(curAction);
}

void ABaseBattlePawn::SkillNameJudgment(const EActionMode curAction)
{
	switch (curAction)
	{
	case EActionMode::Move:
		curSkillName = TEXT("이동!!");
		break;
	case EActionMode::BaseAttack:
		curSkillName = TEXT("타격!!");
		break;
	case EActionMode::Paralysis:
		curSkillName = TEXT("마비 일격!!");
		break;
	case EActionMode::Poison:
		curSkillName = TEXT("맹독 공격!!");
		break;
	case EActionMode::Vulnerable:
		curSkillName = TEXT("취약 타격!!");
		break;
	case EActionMode::Weakening:
		curSkillName = TEXT("약화 일격!!");
		break;
	case EActionMode::Fatal:
		curSkillName = TEXT("치명 일격!!");
		break;
	case EActionMode::Rupture:
		curSkillName = TEXT("파열 일격!!");
		break;
	case EActionMode::Roar:
		curSkillName = TEXT("포효!!");
		break;
	case EActionMode::BattleCry:
		curSkillName = TEXT("전투의 외침!!");
		break;
	default:
		break;
	}
}
void ABaseBattlePawn::UnitRotation(class ABaseBattlePawn* unit)
{
	FVector targetLoc = pathArray[currentPathIndex]->GetActorLocation() + FVector(0, 0, 80);
	FVector currentLoc = GetActorLocation();
	FVector direction = (targetLoc - currentLoc).GetSafeNormal();

	// 메시 회전 부드럽게 보간 처리
	FRotator targetRot = direction.Rotation();
	constexpr float yawOffset = -90.f;
	constexpr float interpSpeed = 8.f;

	// 플레이어 회전 
	if (ABattlePlayer* player = Cast<ABattlePlayer>(unit))
	{
		FRotator currentRot = player->meshComp->GetRelativeRotation();
		FRotator desireRot(0.f, targetRot.Yaw + yawOffset, 0.f);
		smoothRot = FMath::RInterpTo(currentRot, desireRot, GetWorld()->GetDeltaSeconds(), interpSpeed);
		ServerRPC_UnitRotation(smoothRot);
	}
	// Enemy 회전
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(unit))
	{
		FRotator currentRot = enemy->meshComp->GetRelativeRotation();
		FRotator desireRot(0.f, targetRot.Yaw + yawOffset, 0.f);
		smoothRot = FMath::RInterpTo(currentRot, desireRot, GetWorld()->GetDeltaSeconds(),interpSpeed);
		ServerRPC_UnitRotation(smoothRot);
	}
}

void ABaseBattlePawn::ServerRPC_UnitRotation_Implementation(const FRotator& rot)
{
	MultiCastRPC_UnitRotation(rot);
}

void ABaseBattlePawn::MultiCastRPC_UnitRotation_Implementation(const FRotator& rot)
{
	if (auto* player = Cast<ABattlePlayer>(this)) player->meshComp->SetRelativeRotation(rot);
	else if (auto* enemy = Cast<ABaseEnemy>(this)) enemy->meshComp->SetRelativeRotation(rot);
}

void ABaseBattlePawn::UnitMove(class ABaseBattlePawn* unit)
{
	// tile의 로케이션을 가져오고 거기에 z축으로 80만큼 올려서 Player 위치에 맞게 설정
	FVector targetLoc = pathArray[currentPathIndex]->GetActorLocation() + FVector(0, 0, 80);
	FVector currentLoc = GetActorLocation();
	
	// 위치 보간 이동
	newLoc = FMath::VInterpConstantTo(currentLoc, targetLoc, GetWorld()->GetDeltaSeconds(), 500.f);
	// ServerRPC_UnitMove(newLoc); *

	// 플레이어 : RPC로 서버 동기화 요청
	if (IsLocallyControlled())
	{
		ServerRPC_UnitMove(newLoc);
	}
	// 적 유닛 : 서버에서 직접 이동
	else if (HasAuthority())
	{
		SetActorLocation(newLoc);
	}
	
	if (FVector::DistSquared(currentLoc, targetLoc) < FMath::Square(5.f))
	{
		currentPathIndex++;
		if (!pathArray.IsValidIndex(currentPathIndex))
		{
			if (auto* player = Cast<ABattlePlayer>(this))
			{
				player->currentActionMode = EActionMode::None;
				if (IsLocallyControlled()) player->Server_UpdatePlayerAnim(player->currentActionMode);
				else if (HasAuthority()) player->MultiCastRPC_UpdatePlayerAnim(player->currentActionMode);
			}
			else if (auto* enemy = Cast<ABaseEnemy>(this))
			{
				enemy->currentActionMode = EActionMode::None;
				enemy->ServerRPC_UpdateEnemyAnim(enemy->currentActionMode);
			}
			bIsMoving = false;
			currentTile = pathArray.Last();
			pathArray.Empty();

			if (auto* enemy = Cast<ABaseEnemy>(this))
			{
				enemy->FindAndAttackPlayer();
			}
		}
	}
}

void ABaseBattlePawn::ServerRPC_UnitMove_Implementation(const FVector& loc)
{
	MultiCastRPC_UnitMove(loc);
}

void ABaseBattlePawn::MultiCastRPC_UnitMove_Implementation(const FVector& loc)
{
	SetActorLocation(loc);
}

void ABaseBattlePawn::UnitAttackBeforeRoatation(class ABaseBattlePawn* unit)
{
		directionToTarget = (attackTarget->GetActorLocation() -GetActorLocation()).GetSafeNormal2D();
		desiredRot = directionToTarget.Rotation();
		constexpr float interpSpeed = 5.f;
		// 캐릭터의 forward 기준에 맞게 조정
		constexpr float yawOffset = -90.f;

		// 현재 유닛이 Player라면
		if (ABattlePlayer* player = Cast<ABattlePlayer>(this))
		{
			if (player->meshComp)
			{
				FRotator currentRot = player->meshComp->GetRelativeRotation();
				targetMeshRot = FRotator(0.f, desiredRot.Yaw + yawOffset, 0.f);
				newRot = FMath::RInterpTo(currentRot, targetMeshRot, GetWorld()->GetDeltaSeconds(),interpSpeed);
				player->meshComp->SetRelativeRotation(newRot);

				// 회전이 다 됐는지 체크
				if (FMath::Abs(FMath::FindDeltaAngleDegrees(newRot.Yaw, targetMeshRot.Yaw)) < 1.f)
				{
					UnitAttack(player);
				}
			}
		}
		else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(this))
		{
			if (enemy->meshComp)
			{
				FRotator currentRot = enemy->meshComp->GetRelativeRotation();
				targetMeshRot = FRotator(0.f, desiredRot.Yaw + yawOffset, 0.f);
				newRot = FMath::RInterpTo(currentRot, targetMeshRot, GetWorld()->GetDeltaSeconds(),interpSpeed);
				enemy->meshComp->SetRelativeRotation(newRot);

				if (FMath::Abs(FMath::FindDeltaAngleDegrees(newRot.Yaw, targetMeshRot.Yaw)) < 1.f)
				{
					UnitAttack(enemy);
				}
			}
		}
}

void ABaseBattlePawn::UnitAttack(class ABaseBattlePawn* unit)
{
	bWantsToAttack = false;

	if (auto* player = Cast<ABattlePlayer>(unit))
	{
		if (player->bStartMontage)
		{
			// 플레이어 공격 동기화
			Server_PlayerBaseAttack(player);
		}
	}
	else if (auto* enemy = Cast<ABaseEnemy>(unit))
	{
		// bWantsToAttack = false;
		if (enemy->bStartMontage)
		{
			Server_EnemyBaseAttack(enemy);
		}
	}
}

void ABaseBattlePawn::Server_PlayerBaseAttack_Implementation(class ABattlePlayer* player)
{
	Multicast_PlayerBaseAttack(player);
}

void ABaseBattlePawn::Multicast_PlayerBaseAttack_Implementation(class ABattlePlayer* player)
{
	NET_PRINTLOG(TEXT("playerBaseAttack"));
	// 회전 끝나고 몽타주 실행
	player->playerAnim->PlayerAttackAnimation(TEXT("StartBaseAttack"));
	player->bStartMontage = false;
	UE_LOG(LogTemp, Warning, TEXT("Play BaseAttack"));

	// 어떤 스킬 사용했는지
	if (player->curSkillName == "")
	{
		NET_PRINTLOG(TEXT("player->curSkillName == \"\" "));
		return;
	}
	
	player->battleUnitStateComp->SetDrawSize(FVector2D(150, 100));
	player->battleUnitStateUI->SetPrintSkillName(player->curSkillName);
	player->battleUnitStateUI->ShowPrintSkillNameUI();

	FTimerHandle skillNameUIHandle;
	GetWorld()->GetTimerManager().ClearTimer(skillNameUIHandle);
	GetWorld()->GetTimerManager().SetTimer(skillNameUIHandle, [player]()
	{
		if (player && player->battleUnitStateUI)
		{
			player->battleUnitStateUI->ShowBaseUI();
			player->battleUnitStateComp->SetDrawSize(FVector2D(50, 10));
		}
	}, 2.0f, false);
}

void ABaseBattlePawn::Server_EnemyBaseAttack_Implementation(class ABaseEnemy* enemy)
{
	Multicast_EnemyBaseAttack(enemy);
}

void ABaseBattlePawn::Multicast_EnemyBaseAttack_Implementation(class ABaseEnemy* enemy)
{
	// 회전 끝나고 몽타주 실행
	if (enemy->enemyAnim)
	{
		enemy->enemyAnim->PlayBaseAttackAnimation(TEXT("StartBaseAttack"));
		UE_LOG(LogTemp, Warning, TEXT("EnemyBaseAttackAnimation"));

		// 어떤 스킬 사용했는지
		if (enemy->curSkillName == "")
		{
			return;
		}
		enemy->battleUnitStateComp->SetDrawSize(FVector2D(150, 100));
		enemy->battleUnitStateUI->SetPrintSkillName(enemy->curSkillName);
		enemy->battleUnitStateUI->ShowPrintSkillNameUI();

		FTimerHandle skillNameUIHandle;
		GetWorld()->GetTimerManager().ClearTimer(skillNameUIHandle);
		GetWorld()->GetTimerManager().SetTimer(skillNameUIHandle, [enemy]()
		{
			if (enemy && enemy->battleUnitStateUI)
			{
				enemy->battleUnitStateUI->ShowBaseUI();
				enemy->battleUnitStateComp->SetDrawSize(FVector2D(50, 10));
			}
		}, 2.0f, false);
	}
	else
	{
		UE_LOG(LogTemp, Error,TEXT("enemy->enemyAnim is nullptr!"));
	}
	enemy->bStartMontage = false;
	UE_LOG(LogTemp, Warning, TEXT("Play BaseAttack"));
}

void ABaseBattlePawn::InitEnemyState()
{
	if (!(battleUnitStateUI && phaseManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("InitEnemyState : !(battleUnitStateUI && phaseManager"));
		return;
	}

	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(this))
	{
		// 애너미 랜덤 성격 세팅
		InitEnemyTraits();
		// 애너미 스킬 세팅
		InitEnemySkills();
		
		for (const FString trait : enemy->traits)
		{
			UE_LOG(LogTemp, Warning, TEXT("Trait : %s"), *trait);
		}
		for (const FString skill : enemy->skills)
		{
			UE_LOG(LogTemp, Warning, TEXT("Skill : %s"), *skill);
		}
		// 기본 스텟 설정(엔트)
		enemy->hp = 90;
		enemy->attack = 10;
		enemy->defense = 5;
		enemy->resistance = 2;
		enemy->moveRange = 3;
		enemy->critical_Rate = 1.0f;
		enemy->critical_Damage = 1.5f;
		enemy->speed = 3;

		// 성격에 따른 값 추가 세팅
		ApplyTraitModifiers(enemy);

		// 최대 체력 설정
		enemy->maxHP = enemy->hp;
		
		// enemy에 moveRange 세팅 작업
		enemy->moveRange = FMath::Max(1, enemy->moveRange);
		
		UE_LOG(LogTemp, Warning,TEXT("Enemy State Set hp : %d, atk : %d, def : %d, res : %d, mov : %d, "", crit : %f, crit_Damge : %f, spd : %d"),
			   enemy->hp,
			   enemy->attack,
			   enemy->defense,
			   enemy->resistance,
			   enemy->moveRange,
			   enemy->critical_Rate,
			   enemy->critical_Damage,
			   enemy->speed);
		
		// enemy 이름 세팅
		MultiCastRPC_SetEnemyName_Implementation(enemy);
	}
}

void ABaseBattlePawn::InitEnemyTraits()
{
	if (auto* enemy = Cast<ABaseEnemy>(this))
	{
		for (const FString trait : enemyTraits)
		{
			int32 seletTrait = FMath::RandRange(0, 19);
			if (seletTrait <= 3 && !enemy->traits.Contains(trait))
			{
				enemy->traits.Add(trait);
			}
			// 성격 개수가 3개라면 return
			if (enemy->traits.Num() >= 3) return;
		}
		// 개수가 3개보다 작으면 다시 Init을 실행
		if (enemy->traits.Num() < 3)
		{
			InitEnemyTraits();
		}
	}
}

void ABaseBattlePawn::InitEnemySkills()
{
	// 공통 스킬 2개 다른 스킬 2개
	if (auto* enemy = Cast<ABaseEnemy>(this))
	{
		// 엔트
		enemy->skills.Add(TEXT("타격"));
		enemy->skills.Add(TEXT("치명 일격"));
		enemy->skills.Add(TEXT("맹독 공격"));
		enemy->skills.Add(TEXT("취약 타격"));
	}
}

void ABaseBattlePawn::ApplyTraitModifiers(ABaseEnemy* enemy)
{
	for (const FString& trait : enemy->traits)
	{
		if (trait == TEXT("강인함"))
		{
			enemy->hp *= 1.3f;
			enemy->attack *= 0.9f;
		}
		else if (trait == TEXT("잔잔함"))
		{
			enemy->hp *= 1.3f;
			enemy->moveRange -= 1;
		}
		else if (trait == TEXT("호전적"))
		{
			enemy->attack *= 1.2f;
			enemy->critical_Rate *= 0.85f;
		}
		else if (trait == TEXT("충동적"))
		{
			enemy->attack *= 1.2f;
			enemy->defense *= 0.9f;
		}
		else if (trait == TEXT("수비적"))
		{
			enemy->defense *= 1.2f;
			enemy->speed *= 0.9f;
		}
		else if (trait == TEXT("신중함"))
		{
			enemy->defense *= 1.2f;
			enemy->critical_Rate *= 0.85f;
		}
		else if (trait == TEXT("관찰꾼"))
		{
			enemy->critical_Rate *= 1.3f;
			enemy->attack *= 0.9f;
		}
		else if (trait == TEXT("잔인함"))
		{
			enemy->critical_Rate *= 1.3f;
			enemy->speed *= 0.9f;
		}
		else if (trait == TEXT("겁쟁이"))
		{
			enemy->moveRange += 2;
			enemy->attack *= 0.9f;
		}
		else if (trait == TEXT("허세꾼"))
		{
			enemy->moveRange += 2;
			enemy->defense *= 0.9f;
		}
		else if (trait == TEXT("교란꾼"))
		{
			enemy->speed *= 1.2f;
			enemy->attack *= 0.9f;
		}
		else if (trait == TEXT("파괴적"))
		{
			enemy->speed *= 1.2f;
			enemy->defense *= 0.9f;
		}
		else if (trait == TEXT("협동적"))
		{
			enemy->hp *= 1.15f;
			enemy->defense *= 1.1f;
			enemy->speed *= 0.9f;
		}
		else if (trait == TEXT("용감함"))
		{
			enemy->attack *= 1.1f;
			enemy->defense *= 1.1f;
			enemy->critical_Rate *= 0.85f;
		}
		else if (trait == TEXT("조화적"))
		{
			enemy->attack *= 1.1f;
			enemy->defense *= 1.1f;
			enemy->speed *= 0.9f;
		}
		else if (trait == TEXT("고립적"))
		{
			enemy->attack *= 1.1f;
			enemy->speed *= 1.1f;
			enemy->defense *= 0.9f;
		}
		else if (trait == TEXT("지능적"))
		{
			enemy->defense *= 1.1f;
			enemy->critical_Rate *= 1.15f;
			enemy->moveRange -= 1;
		}
		else if (trait == TEXT("냉정함"))
		{
			enemy->defense *= 1.1f;
			enemy->critical_Rate *= 1.15f;
			enemy->speed *= 0.9f;
		}
		else if (trait == TEXT("원한꾼"))
		{
			enemy->critical_Rate *= 1.15f;
			enemy->speed *= 1.1f;
			enemy->defense *= 0.9f;
		}
		else if (trait == TEXT("민첩함"))
		{
			enemy->moveRange += 1;
			enemy->speed *= 1.1f;
			enemy->defense *= 0.9f;
		}
	}
}

void ABaseBattlePawn::InitOtherClassPointer()
{
	gi = Cast<UWorldGi>(GetWorld()->GetGameInstance());
	if (gi)
	{
		UE_LOG(LogTemp, Warning, TEXT("GI is Set"));
	}
	// PhaseManager
	phaseManager = Cast<AUPhaseManager>(GetWorld()->GetGameState());
	if (phaseManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("phaseManager is Set"));
	}
	// TurnManager
	turnManager = Cast<ATurnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), turnManagerFactory));
	if (turnManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("turnManager is Set"));
	}
	// PC
	pc = GetWorld()->GetFirstPlayerController();
	if (pc)
	{
		UE_LOG(LogTemp, Warning, TEXT("PC is Set"));

		// hud 포인터 세팅
		hud = Cast<ABattleHUD>(pc->GetHUD());
		if (hud) UE_LOG(LogTemp, Warning, TEXT("hud is Set"));
	}
	// girdTile Setting
	gridTileManager = Cast<AGridTileManager>(UGameplayStatics::GetActorOfClass(GetWorld(), TileManagerFactory));
	if (gridTileManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("gridTileManager is Set"));
	}
	// BaseHP UI 보여주기
	battleUnitStateUI = Cast<UBattleUnitStateUI>(this->battleUnitStateComp->GetWidget());
	if (battleUnitStateUI)
	{
		UE_LOG(LogTemp, Warning,TEXT("battleUnitStateUI is Set"));
	}
}
void ABaseBattlePawn::ServerInitOtherClassPointer_Implementation()
{
	MulticastInitOtherClassPointer();
}

void ABaseBattlePawn::MulticastInitOtherClassPointer_Implementation()
{
	InitOtherClassPointer();
}

void ABaseBattlePawn::InitAPUI()
{
	if (!(pc && hud && hud->mainUI && hud->mainUI->HB_AP))
	{
		UE_LOG(LogTemp, Warning,TEXT("InitAPUI is Not Set : pc && hud && hud->mainUI && hud->mainUI->HB_AP"));
		return;
	}

	// AP UI 초기화
	hud->mainUI->HB_AP->ClearChildren();

	// 현재 턴 유닛이 Player인 경우에만
	if (ABattlePlayer* player = Cast<ABattlePlayer>(turnManager->curUnit))
	{
		NET_PRINTLOG(TEXT("Player name : %s, turnManager->curUnit : %d"), *player->GetActorNameOrLabel(), player->curAP);
		for (int32 i = 0; i < player->curAP; ++i)
		{
			hud->mainUI->AddAP(); // AP UI 하나씩 추가
		}
	}
}

void ABaseBattlePawn::ClientRPC_AddAP_Implementation(class ABattlePlayer* player)
{
	if (IsLocallyControlled()) player->GetAP();
}

void ABaseBattlePawn::MultiCastRPC_InitAPUI_Implementation()
{
	InitAPUI();
}

void ABaseBattlePawn::MultiCastRPC_SetMyName_Implementation(
	int32 Count)
{
	MyName = FString("NyName : ") + FString::FromInt(Count);
}

void ABaseBattlePawn::ShowDialoge_Implementation(const FString& dialogue)
{
	// if (HasAuthority())
	// 	return;
	MultiCastRPC_ShowDialoge(dialogue);
}

void ABaseBattlePawn::MultiCastRPC_ShowDialoge_Implementation(const FString& dialogue)
{
	if (hud && hud->mainUI)
	{
		hud->mainUI->AddReason(dialogue);
	}
}

void ABaseBattlePawn::MultiCastRPC_UpdateReason_Implementation()
{
	// EnemyPhaseEnd
	if (hud && hud->mainUI)
	{
		hud->mainUI->SB_Reason->ClearChildren();
	}
}

void ABaseBattlePawn::MultiCastRPC_ClearGridTile_Implementation()
{
	// 타일 범위 띄워 놨었다면 지우기
	this->ClearGridTile();
}
