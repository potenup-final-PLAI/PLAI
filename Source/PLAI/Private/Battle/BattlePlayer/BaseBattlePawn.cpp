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
#include "Battle/UI/WorldDamageUI.h"
#include "Battle/UI/WorldDamageUIActor.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/HorizontalBox.h"
#include "Components/WidgetComponent.h"
#include "Enemy/BaseEnemy.h"
#include "Enemy/BattleEnemyAnimInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/BattlePlayer.h"
#include "Player/BattlePlayerAnimInstance.h"
#include "Player/BattlePlayerState.h"

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
	cameraComp->SetRelativeLocationAndRotation(FVector(-600, 0, 1000),
	                                           FRotator(-60, 0, 0));

	// 이름, HP, Armor 설정 컴포넌트
	battleUnitStateComp = CreateDefaultSubobject<UWidgetComponent>(
		"BattleUnitStateComp");
	battleUnitStateComp->SetupAttachment(RootComponent);
	battleUnitStateComp->SetDrawSize(FVector2D((0)));
	battleUnitStateComp->SetRelativeLocationAndRotation(
		FVector(0, 0, 170), FRotator(0));
	battleUnitStateComp->SetGenerateOverlapEvents(false); // 충돌 막기
	battleUnitStateComp->SetVisibility(true);
	battleUnitStateComp->SetCastShadow(false);

	ConstructorHelpers::FClassFinder<UBattleUnitStateUI> tempBattleUnitStateUI(
		TEXT(
			"/Script/UMGEditor.WidgetBlueprint'/Game/JS/UI/WBP_BattleUnitState.WBP_BattleUnitState_C'"));
	if (tempBattleUnitStateUI.Succeeded())
	{
		battleUnitStateComp->SetWidgetClass(tempBattleUnitStateUI.Class);
	}
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

// Called every frame
void ABaseBattlePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Hover UI 띄우기 위한 함수
	OnMouseHover();

	// Unit 이동, 회전, 공격
	UnitMoveRotateAttack();

	// UI 빌보드 처리
	BillboardBattleUnitStateUI();
}

void ABaseBattlePawn::OnTurnStart()
{
	if (!turnManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("turnManager is Not Set"));
		return;
	}
	// 턴 종료 초기화
	bTurnEnded = false;

	// 기본 공격 초기화
	bBaseAttack = true;

	// 턴이 시작됐으면 턴 카운트 1 증가
	turnManager->turnCount++;
	UE_LOG(LogTemp, Warning, TEXT("Turn Count %d"), turnManager->turnCount);

	// 턴 유닛 이름 UI에 세팅
	if (pc && hud && hud->mainUI && hud->mainUI->WBP_CycleAndTurn)
	{
		FString s = Cast<ABattlePlayer>(turnManager->curUnit)
			            ? TEXT("Player")
			            : TEXT("Enemy");
		hud->mainUI->WBP_CycleAndTurn->SetTurnText(s);
	}

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

	// 상태이상이 있다면 대미지 및 버프, 디버프 처리하기
	ApplyStatusEffect();

	// Player라면
	if (ABattlePlayer* player = Cast<ABattlePlayer>(this))
	{
		player->GetAP();
		InitAPUI();
		UE_LOG(LogTemp, Warning, TEXT("%s -> curAP : %d"), *player->GetName(),
		       player->curAP);
	}
	// Enemy라면
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(this))
	{
		if (!enemybattleState)
		{
			UE_LOG(LogTemp, Error, TEXT("enemybattleState is null on OnTurnStart"));
			return;
		}
		UE_LOG(LogTemp, Warning, TEXT("BaseBattlePawn::OnTurnStart"));

		// 턴 시작 시 AP 증가
		enemybattleState->curAP += enemybattleState->GetAP(enemybattleState->curAP);
		UE_LOG(LogTemp, Warning, TEXT("Current AP: %d"),enemybattleState->curAP);

		enemy->SeeMoveRange(enemy->enemybattleState->move_Range);
		
		ABaseBattlePawn* CapturedUnit = this;

		FTimerHandle battleAPIHandle;
		GetWorld()->GetTimerManager().SetTimer(battleAPIHandle,FTimerDelegate::CreateLambda([this, CapturedUnit]()
		{
			UE_LOG(LogTemp, Warning,TEXT("BaseBattlePawn::In Lambda"));
			if (turnManager && turnManager->phaseManager)
			{
				UE_LOG(LogTemp,Warning,TEXT("BaseBattlePawn::turnManager, phaseManager is Set"));
				turnManager->phaseManager->TrySendbattleState(CapturedUnit);
			}
		}), 1.0f, false);
	}
	UE_LOG(LogTemp, Warning, TEXT("%s Turn Start"), *GetName());
}

void ABaseBattlePawn::OnTurnEnd()
{
	// 여러 번 호출 방지
	if (bTurnEnded)
	{
		return;
	}
	bTurnEnded = true;
	// 타일 범위 띄워 놨었다면 지우기
	ClearGridTile();
	
	UE_LOG(LogTemp, Warning, TEXT("%s Turn End"), *GetName());
	// 입력 막고 FSM 종료

	// 턴 종료 상태거나 phaseManager가 없다면 return
	if (!phaseManager && !turnManager && phaseManager->turnManager->curTurnState
		== ETurnState::TurnEnd)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT(
			       " OnTurnEnd : !(phaseManager && phaseManager->turnManager->curTurnState == ETurnState::TurnEnd"
		       ));
		return;
	}
	if (ABattlePlayer* player = Cast<ABattlePlayer>(this))
	{
		// PlayerPhaseEnd
		phaseManager->EndPlayerPhase();
	}
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(this))
	{
		// EnemyPhaseEnd
		phaseManager->EndEnemyPhase();
	}
}


void ABaseBattlePawn::OnMouseLeftClick()
{
	FVector start, end, dir;
	FHitResult hitInfo;
	FCollisionQueryParams params;

	GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(start, dir);
	end = start + dir * 10000;

	if (GetWorld()->LineTraceSingleByChannel(hitInfo, start, end,
	                                         ECC_Visibility, params))
	{
		// // 상대와 거리 측정
		// float dist = FVector::Dist(GetActorLocation(),hitInfo.GetActor()->GetActorLocation());
		//
		// // 자신의 moveRange * 100 보다 작거나 같을 때
		// if (dist > (this->battlePlayerState->playerStatus.move_Range * 100))
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("Test Click : Dist too be far"));
		// 	return;
		// }
		if (AGridTile* clickedTile = Cast<AGridTile>(hitInfo.GetActor()))
		{
			if (!highlightedTiles.Contains(clickedTile))
			{
				UE_LOG(LogTemp, Warning, TEXT("클릭한 타일이 이동 범위에 없습니다."));
				return;
			}
		}
		
		EActionMode curSkillState = currentActionMode;
		UE_LOG(LogTemp, Warning, TEXT("Test Click : Current Action Mode %s"),
		       *UEnum::GetValueAsString(curSkillState));
		// UI에 띄울 스킬 이름 저장 
		SkillNameJudgment(curSkillState);
		switch (curSkillState)
		{
		case EActionMode::None:
			break;
		case EActionMode::TurnEnd:
			OnTurnEnd();
			break;
		case EActionMode::Move:
			PlayerMove(hitInfo);
			break;
		case EActionMode::BaseAttack:
			PlayerBaseAttack(hitInfo);
			UE_LOG(LogTemp, Warning, TEXT("OnMouseLeftClick BaseAttack"));
			break;
		case EActionMode::Paralysis:
			UE_LOG(LogTemp, Warning, TEXT("OnMouseLeftClick Paralysis"));
			PlayerParalysis(hitInfo);
			break;
		case EActionMode::Poison:
			PlayerPoison(hitInfo);
			break;
		case EActionMode::Vulnerable:
			PlayerVulnerable(hitInfo);
			break;
		case EActionMode::Weakening:
			PlayerWeaking(hitInfo);
			break;
		case EActionMode::Fatal:
			PlayerFatal(hitInfo);
			break;
		case EActionMode::Rupture:
			PlayerRupture(hitInfo);
			break;
		case EActionMode::Roar:
			PlayerRoar(hitInfo);
			break;
		case EActionMode::BattleCry:
			PlayerBattleCry(hitInfo);
			break;
		}
	}
}

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
		if (auto* player = Cast<ABattlePlayer>(this))
		{
			if (player->playerAnim)
			{
				player->playerAnim->actionMode = currentActionMode;
			}
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

		if (GetWorld()->LineTraceSingleByChannel(
			startHit, start, end, ECC_Visibility, params))
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

void ABaseBattlePawn::PlayerBaseAttack(FHitResult& hitInfo)
{
	if (!bBaseAttack)
	{
		UE_LOG(LogTemp, Warning, TEXT("Use BaseAttack Before"));
		return;
	}

	bBaseAttack = false;

	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		// 타겟 저장
		targetEnemy = enemy;
		bWantsToAttack = true;
		attackTarget = targetEnemy;

		if (auto* player = Cast<ABattlePlayer>(this))
		{
			if (player->playerAnim)
			{
				player->playerAnim->actionMode = currentActionMode;
				player->bStartMontage = true;
			}
		}
	}
	// 공격 대상이 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		// 타겟 저장
		targetPlayer = player;
		bWantsToAttack = true;
		attackTarget = targetPlayer;

		// Anim 업데이트
		if (enemy->enemyAnim)
		{
			enemy->enemyAnim->actionMode = currentActionMode;
		}


		enemy->bStartMontage = true;
	}
}

void ABaseBattlePawn::PlayerParalysis(FHitResult& hitInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerParalysis In"));
	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		if (enemy->enemyAnim)
		{
			enemy->enemyAnim->actionMode = currentActionMode;
		}
		UE_LOG(LogTemp, Warning, TEXT("PlayerParalysis In Enemy"));
		EnemyApplyAttack(enemy, EActionMode::Paralysis);
	}
	// 공격 대상이 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		if (player->playerAnim)
		{
			player->playerAnim->actionMode = currentActionMode;
		}
		UE_LOG(LogTemp, Warning, TEXT("PlayerParalysis In Player"));
		PlayerApplyAttack(player, EActionMode::Paralysis);
	}
}

void ABaseBattlePawn::PlayerPoison(FHitResult& hitInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerPoison In"));
	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		if (enemy->enemyAnim)
		{
			enemy->enemyAnim->actionMode = currentActionMode;
		}
		UE_LOG(LogTemp, Warning, TEXT("Cast Enemy In"));
		PlayerApplyAttack(enemy, EActionMode::Poison);
		
	}
	// 공격 대상이 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		if (player->playerAnim)
		{
			player->playerAnim->actionMode = currentActionMode;
		}
		EnemyApplyAttack(player, EActionMode::Poison);
	}
}

void ABaseBattlePawn::PlayerVulnerable(FHitResult& hitInfo)
{
	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		if (enemy->enemyAnim)
		{
			enemy->enemyAnim->actionMode = currentActionMode;
		}
		EnemyApplyAttack(enemy, EActionMode::Vulnerable);
	}
	// 공격 대상이 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		if (player->playerAnim)
		{
			player->playerAnim->actionMode = currentActionMode;
		}
		PlayerApplyAttack(player, EActionMode::Vulnerable);
	}
}

void ABaseBattlePawn::PlayerWeaking(FHitResult& hitInfo)
{
	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		if (enemy->enemyAnim)
		{
			enemy->enemyAnim->actionMode = currentActionMode;
		}
		EnemyApplyAttack(enemy, EActionMode::Weakening);
	}
	// 공격 대상이 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		if (player->playerAnim)
		{
			player->playerAnim->actionMode = currentActionMode;
		}
		PlayerApplyAttack(player, EActionMode::Weakening);
	}
}

void ABaseBattlePawn::PlayerFatal(FHitResult& hitInfo)
{
	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		if (enemy->enemyAnim)
		{
			enemy->enemyAnim->actionMode = currentActionMode;
		}
		EnemyApplyAttack(enemy, EActionMode::Fatal);
	}
	// 공격 대상이 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		if (player->playerAnim)
		{
			player->playerAnim->actionMode = currentActionMode;
		}
		PlayerApplyAttack(player, EActionMode::Fatal);
	}
}

void ABaseBattlePawn::PlayerRupture(FHitResult& hitInfo)
{
	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		if (enemy->enemyAnim)
		{
			enemy->enemyAnim->actionMode = currentActionMode;
		}
		EnemyApplyAttack(enemy, EActionMode::Rupture);
	}
	// 공격 대상이 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		if (player->playerAnim)
		{
			player->playerAnim->actionMode = currentActionMode;
		}
		PlayerApplyAttack(player, EActionMode::Rupture);
	}
}

void ABaseBattlePawn::PlayerRoar(FHitResult& hitInfo)
{
	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		if (enemy->enemyAnim)
		{
			enemy->enemyAnim->actionMode = currentActionMode;
		}
		EnemyApplyAttack(enemy, EActionMode::Roar);
	}
	// 공격 대상이 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		if (player->playerAnim)
		{
			player->playerAnim->actionMode = currentActionMode;
		}
		PlayerApplyAttack(player, EActionMode::Roar);
	}
}

void ABaseBattlePawn::PlayerBattleCry(FHitResult& hitInfo)
{
	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		if (enemy->enemyAnim)
		{
			enemy->enemyAnim->actionMode = currentActionMode;
		}
		EnemyApplyAttack(enemy, EActionMode::BattleCry);
	}
	// 공격 대상이 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		if (player->playerAnim)
		{
			player->playerAnim->actionMode = currentActionMode;
		}
		PlayerApplyAttack(player, EActionMode::BattleCry);
	}
}


void ABaseBattlePawn::GetDamage(ABaseBattlePawn* unit, int32 damage)
{
	// player HP가 0보다 크다면
	UE_LOG(LogTemp, Warning, TEXT("In GetDamage"));
	if (ABattlePlayer* player = Cast<ABattlePlayer>(unit))
	{
		// 피를 깎는다.
		player->battlePlayerState->playerStatus.hp = FMath::Max(
			0, player->battlePlayerState->playerStatus.hp - damage);
		UE_LOG(LogTemp, Warning, TEXT("Damage : %d, playerHP : %d"), damage,
		       player->battlePlayerState->playerStatus.hp);

		// HP를 UI에 업데이트
		if (!(battleUnitStateUI && pc && hud && hud->mainUI && hud->mainUI->
			WBP_Player && phaseManager && player->playerAnim))
		{
			UE_LOG(LogTemp, Warning,
			       TEXT(
				       "GetDamage(Player) : !(battleUnitStateUI && pc && hud && hud->mainUI && hud->mainUI->WBP_Player)"
			       ));
			return;
		}

		// BattlePlayerInfo HP 업데이트
		player->battleUnitStateUI->UpdateHP(
			player->battlePlayerState->playerStatus.hp);
		player->hud->mainUI->WBP_Player->PlayerUpdateHP(
			player, player->battlePlayerState->playerStatus.hp);

		player->playerAnim->
		        PlayHitMotionAnimation(TEXT("StartPlayerHitMotion"));
		UE_LOG(LogTemp, Warning, TEXT("anim Set! Hit Animation !!"));

		// Actor 위치 옮기고 UI Actor 보이게
		FVector loc = FVector(player->GetActorLocation().X,
		                      player->GetActorLocation().Y,
		                      player->GetActorLocation().Z + 250);
		phaseManager->damageUIActor->SetActorLocation(loc);
		phaseManager->damageUIActor->ShowDamageUI();
		// 대미지 텍스트 변경
		phaseManager->damageUIActor->damageUI->SetDamageText(damage);

		// hp가 0보다 작으면 사망
		if (player->battlePlayerState->playerStatus.hp <= 0)
		{
			// player에 세팅
			player->battlePlayerState->playerLifeState = ELifeState::Dead;
			// playerAnim에 세팅
			player->playerAnim->lifeState = ELifeState::Dead;
			UE_LOG(LogTemp, Warning, TEXT("Enemy Dead %s"),
			       *UEnum::GetValueAsString(player->battlePlayerState->
				       playerLifeState));
		}
	}
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(unit))
	{
		if (!(enemy->enemyAnim && phaseManager))
		{
			UE_LOG(LogTemp, Warning,
			       TEXT(
				       "GetDamage(Enemy) : !(battleUnitStateUI && pc && hud && hud->mainUI && hud->mainUI->WBP_Player)"
			       ));
			return;
		}
		// 피를 깎는다.
		enemy->enemybattleState->hp = FMath::Max(
			0, enemy->enemybattleState->hp - damage);
		UE_LOG(LogTemp, Warning, TEXT("Damage : %d, enemyHP : %d"), damage,
		       enemy->enemybattleState->hp);
		// UI HP 업데이트
		enemy->battleUnitStateUI->UpdateHP(enemy->enemybattleState->hp);

		// 공격 몽타주 실행
		enemy->enemyAnim->PlayHitMotionAnimation(TEXT("StartHitMotion"));
		UE_LOG(LogTemp, Warning, TEXT("anim Set! Hit Animation !!"));

		// Actor 위치 옮기고 UI Actor 보이게
		FVector loc = FVector(enemy->GetActorLocation().X,
		                      enemy->GetActorLocation().Y,
		                      enemy->GetActorLocation().Z + 250);
		phaseManager->damageUIActor->SetActorLocation(loc);
		phaseManager->damageUIActor->ShowDamageUI();
		// 대미지 텍스트 변경
		phaseManager->damageUIActor->damageUI->SetDamageText(damage);

		// hp가 0보다 작으면 사망
		if (enemy->enemybattleState->hp <= 0)
		{
			// enemy 자체에 State 세팅
			enemy->enemybattleState->enemyLifeState = ELifeState::Dead;
			// enemy Anim에 State 세팅
			enemy->enemyAnim->lifeState = ELifeState::Dead;
			UE_LOG(LogTemp, Warning, TEXT("Enemy Dead %s"),
			       *UEnum::GetValueAsString(enemy->enemybattleState->
				       enemyLifeState));
		}
	}

	// 대미지를 줬다면 현재 공격 상태를 None으로 초기화
	if (auto* player = Cast<ABattlePlayer>(this))
	{
		if (auto* anim = Cast<UBattlePlayerAnimInstance>(
			player->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
	else if (auto* enemy = Cast<ABaseEnemy>(this))
	{
		if (auto* anim = Cast<UBattleEnemyAnimInstance>(
			enemy->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
}
void ABaseBattlePawn::PlayerApplyAttack(ABaseBattlePawn* targetUnit,
                                        EActionMode attackType)
{
	auto* player = Cast<ABattlePlayer>(this);
	auto* enemy = Cast<ABaseEnemy>(targetUnit);
	if (!player || !enemy)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Player Attack Enemy"));
	int32 atk = player->battlePlayerState->playerStatus.attack;
	int32 weapon = 0;
	float critical = player->battlePlayerState->playerStatus.critical_Rate;
	float criticalDamage = player->battlePlayerState->playerStatus.
	                               critical_Damage;
	int32 personality = 0;
	int32 status_effect = 0;
	int32 damage = 0;

	// 스킬 계수 및 추가 효과
	float skillMultiplier = 1.0f;

	switch (attackType)
	{
	case EActionMode::Paralysis:
		// 현재 Ap가 cost보다 크다면 실행
		UE_LOG(LogTemp, Warning, TEXT("In Paralysis"));
		if (!player->CanConsumeAP(1))
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
			return;
		}
		skillMultiplier = 0.8f;
	// 상대에게 상태이상 및 몇 턴동안 진행 될지 추가
		player->AddStatusEffect(EStatusEffect::Weakening, 2);
		break;
	case EActionMode::Poison:
		if (!player->CanConsumeAP(1))
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
			return;
		}
		player->AddStatusEffect(EStatusEffect::Poison, 3);
		break;
	case EActionMode::Vulnerable:
		if (!player->CanConsumeAP(1))
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
			return;
		}
		player->AddStatusEffect(EStatusEffect::Vulnerable, 2);
		break;
	case EActionMode::Weakening:
		if (!player->CanConsumeAP(1))
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
			return;
		}
		player->AddStatusEffect(EStatusEffect::Weakening, 2);
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
	case EActionMode::Roar:
		if (!player->CanConsumeAP(1))
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
			return;
		}
		player->AddStatusEffect(EStatusEffect::Weakening, 2);
		break;
	case EActionMode::BattleCry:
		if (!player->CanConsumeAP(1))
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
			return;
		}
	// 자신과 동료들에게 버프 부여
	// 주위 동료들에게 피해 증가 버프 부여
	// 작성해야함 레벨에 있는 unit들 다 모아서 enemy일 때 반복문으로 버프주면 될듯
		player->AddStatusEffect(EStatusEffect::Angry, 1);
		break;
	default:
		// 기본 타격 스킬 배율
		skillMultiplier = 1.0f;
		break;
	}

	// AP 감소 UI에 반영
	InitAPUI();

	// Rour와 BattleCry는 비공격스킬 및 버프스킬이므로 return 시키면 될듯?
	if (attackType == EActionMode::Roar || attackType ==
		EActionMode::BattleCry)
	{
		return;
	}


	CalculateAndApplyDamage(enemy, atk, weapon, skillMultiplier, critical, criticalDamage, personality, status_effect);

	// 대미지를 줬다면 현재 공격 상태를 None으로 초기화
	currentActionMode = EActionMode::None;
	if (player->playerAnim)
	{
		player->playerAnim->actionMode = currentActionMode;
	}
}
void ABaseBattlePawn::EnemyApplyAttack(ABaseBattlePawn* targetUnit,
                                       EActionMode attackType)
{
	auto* player = Cast<ABattlePlayer>(targetUnit);
	auto* enemy = Cast<ABaseEnemy>(this);
	if (!player || !enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("!player || !enemy"));
		return;
	}

	UE_LOG(LogTemp, Warning,
	       TEXT("[EnemyApplyAttack] Enemy %s attacking Player %s"),
	       *enemy->GetName(), *player->GetName());

	int32 atk = enemybattleState->attack;
	int32 weapon = 0;
	float critical = enemybattleState->critical_Rate;
	float criticalDamage = enemybattleState->critical_Damage;
	int32 personality = 0;
	int32 status_effect = 0;
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
		// EnemySkillList에서 고유 스킬 처리
		// HandleEnemyUniqueSkill(enemy, player, attackType);
		break;
	}

	// 대미지 처리
	CalculateAndApplyDamage(player, atk, weapon, skillMultiplier, critical, criticalDamage, personality, status_effect);

	// 대미지를 줬다면 None으로 변경
	currentActionMode = EActionMode::None;
	if (enemy->enemyAnim)
	{
		enemy->enemyAnim->actionMode = currentActionMode;
	}
}

void ABaseBattlePawn::CalculateAndApplyDamage(ABaseBattlePawn* target,
                                              int32 atk, int32 weapon,
                                              float skillMultiplier,
                                              float criticalRate,
                                              float criticalDamage,
                                              int32 personality,
                                              int32 status_effect)
{
	UE_LOG(LogTemp, Warning, TEXT("In Calculate"));
	int32 damage = 0;
	int32 chance = FMath::RoundToInt(criticalRate * 100.0f);
	int32 roll = FMath::RandRange(1, 100);

	if (bool bIsCrit = roll <= chance)
	{
		damage = ((atk + weapon) * skillMultiplier * (1 + (personality + status_effect))) * criticalDamage;
		UE_LOG(LogTemp, Warning, TEXT("Critical Damage : %d"), damage);
	}
	else
	{
		damage = (atk + weapon) * skillMultiplier * (1 + (personality + status_effect));
		UE_LOG(LogTemp, Warning, TEXT("Damage : %d"), damage);
	}

	GetDamage(target, damage);
}

void ABaseBattlePawn::AddStatusEffect(EStatusEffect newEffect, int32 duration)
{
	if (activeStatusEffects.Contains(newEffect))
	{
		// 이미 있으면 지속시간 갱신 or 덧붙이기
		activeStatusEffects[newEffect] = FMath::Max(
			activeStatusEffects[newEffect], duration);
		// UEnum로그를 찍기 위해 enum 메타데이터를 얻어서 Log찍는 과정 
		UEnum* enumPtr = StaticEnum<EStatusEffect>();
		if (enumPtr) UE_LOG(LogTemp, Warning,
		                    TEXT("In AddStatusEffect : %s, Duration : %d"),
		                    *enumPtr->GetNameStringByValue(static_cast<int64>(
			                    newEffect)), activeStatusEffects[newEffect]);
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
		UE_LOG(LogTemp, Warning,
		       TEXT(
			       "HandleStatus : !battleUnitStateUI && pc && hud && hud->mainUI && hud->mainUI->WBP_Player"
		       ));
		return;
	}
	switch (effect)
	{
	case EStatusEffect::Poison:
		if (auto* player = Cast<ABattlePlayer>(this))
		{
			player->battlePlayerState->playerStatus.hp -= 5;
			battleUnitStateUI->UpdateHP(
				player->battlePlayerState->playerStatus.hp);
			// BattlePlayerInfo UI 세팅
			hud->mainUI->WBP_Player->SetPlayerHPUI(player);
		}
		else if (auto* enemy = Cast<ABaseEnemy>(this))
		{
			enemy->enemybattleState->hp -= 5;
			battleUnitStateUI->UpdateHP(enemy->enemybattleState->hp);
		}
		break;
	case EStatusEffect::Vulnerable:
		if (auto* player = Cast<ABattlePlayer>(this))
		{
			VulnerableProcess(player->battlePlayerState);
		}
		else if (auto* enemy = Cast<ABaseEnemy>(this))
		{
			VulnerableEnemyProcess(enemy->enemybattleState);
		}
		break;
	case EStatusEffect::Weakening:
		if (auto* player = Cast<ABattlePlayer>(this))
		{
			WeakeningProcess(player->battlePlayerState);
		}
		else if (auto* enemy = Cast<ABaseEnemy>(this))
		{
			WeakeningEnemyProcess(enemy->enemybattleState);
		}
		break;
	case EStatusEffect::Angry:
		if (auto* player = Cast<ABattlePlayer>(this))
		{
			AngryProcess(player->battlePlayerState);
		}
		else if (auto* enemy = Cast<ABaseEnemy>(this))
		{
			AngryEnemyProcess(enemy->enemybattleState);
		}
		break;
	case EStatusEffect::Bleeding:
		if (auto* player = Cast<ABattlePlayer>(this))
		{
			player->battlePlayerState->playerStatus.hp -= 5;
			battleUnitStateUI->UpdateHP(
				player->battlePlayerState->playerStatus.hp);

			// BattlePlayerInfo UI 세팅
			hud->mainUI->WBP_Player->SetPlayerHPUI(player);
			// 출혈로 인한 스텟 상태 변경 로직 
			BleedingProcess(player->battlePlayerState);
		}
		else if (auto* enemy = Cast<ABaseEnemy>(this))
		{
			enemy->enemybattleState->hp -= 5;
			battleUnitStateUI->UpdateHP(enemy->enemybattleState->hp);
			BleedingEnemyProcess(enemy->enemybattleState);
		}
		break;
	default:
		break;
	}
}

void ABaseBattlePawn::WeakeningProcess(ABattlePlayerState* playerState)
{
	// 약화 상태이상 처리 프로세스
	// 공격력과 방어력 스탯 감소
	int32& atk = playerState->playerStatus.attack;
	int32& def = playerState->playerStatus.defense;
	// 반올림 처리
	int32 decreaseAtkAmount = FMath::RoundToInt(atk * 0.2f);
	int32 decreaseDefAmount = FMath::RoundToInt(def * 0.2f);
	// 음수 방지
	atk = FMath::Max(0, atk - decreaseAtkAmount);
	def = FMath::Max(0, def - decreaseDefAmount);
	UE_LOG(LogTemp, Warning, TEXT("WeakeningProcess : %d"), atk);
	UE_LOG(LogTemp, Warning, TEXT("WeakeningProcess : %d"), def);
}

void ABaseBattlePawn::VulnerableProcess(ABattlePlayerState* playerState)
{
	// 방어력 감소 처리
	int32& def = playerState->playerStatus.defense;
	int32 decreaseDefAmount = FMath::RoundToInt(def * 0.5f);
	def = FMath::Max(0, def - decreaseDefAmount);
	UE_LOG(LogTemp, Warning, TEXT("VulnerableProcess : %d"), def);
}

void ABaseBattlePawn::AngryProcess(ABattlePlayerState* playerState)
{
	// 공격력 증가 처리
	int32& atk = playerState->playerStatus.attack;
	int32 decreaseAtkAmount = FMath::RoundToInt(atk * 0.5f);
	atk = FMath::Max(0, atk - decreaseAtkAmount);
	UE_LOG(LogTemp, Warning, TEXT("WeakeningProcess : %d"), atk);
}

void ABaseBattlePawn::BleedingProcess(ABattlePlayerState* playerState)
{
	// 방어력 감소 처리
	int32& def = playerState->playerStatus.defense;
	int32 decreaseDefAmount = FMath::RoundToInt(def * 0.2f);
	def = FMath::Max(0, def - decreaseDefAmount);
	UE_LOG(LogTemp, Warning, TEXT("VulnerableProcess : %d"), def);
}

void ABaseBattlePawn::WeakeningEnemyProcess(UEnemyBattleState* enemyState)
{
	// 약화 상태이상 처리 프로세스
	// 공격력과 방어력 스탯 감소
	int32& atk = enemyState->attack;
	int32& def = enemyState->defense;
	// 반올림 처리
	int32 decreaseAtkAmount = FMath::RoundToInt(atk * 0.2f);
	int32 decreaseDefAmount = FMath::RoundToInt(def * 0.2f);
	// 음수 방지
	atk = FMath::Max(0, atk - decreaseAtkAmount);
	def = FMath::Max(0, def - decreaseDefAmount);
	UE_LOG(LogTemp, Warning, TEXT("WeakeningProcess : %d"), atk);
	UE_LOG(LogTemp, Warning, TEXT("WeakeningProcess : %d"), def);
}

void ABaseBattlePawn::VulnerableEnemyProcess(UEnemyBattleState* enemyState)
{
	// 방어력 감소 처리
	int32& def = enemyState->defense;
	int32 decreaseDefAmount = FMath::RoundToInt(def * 0.5f);
	def = FMath::Max(0, def - decreaseDefAmount);
	UE_LOG(LogTemp, Warning, TEXT("VulnerableProcess : %d"), def);
}

void ABaseBattlePawn::AngryEnemyProcess(UEnemyBattleState* enemyState)
{
	// 공격력 증가 처리
	int32& atk = enemyState->attack;
	int32 decreaseAtkAmount = FMath::RoundToInt(atk * 0.5f);
	atk = FMath::Max(0, atk - decreaseAtkAmount);
	UE_LOG(LogTemp, Warning, TEXT("WeakeningProcess : %d"), atk);
}

void ABaseBattlePawn::BleedingEnemyProcess(UEnemyBattleState* enemyState)
{
	// 방어력 감소 처리
	int32& def = enemyState->defense;
	int32 decreaseDefAmount = FMath::RoundToInt(def * 0.2f);
	def = FMath::Max(0, def - decreaseDefAmount);
	UE_LOG(LogTemp, Warning, TEXT("VulnerableProcess : %d"), def);
}

void ABaseBattlePawn::PathFind()
{
	UE_LOG(LogTemp, Warning,
	       TEXT("BuildPath: pathArray.Num = %d, moveRange = %d"),
	       pathArray.Num(), moveRange);
	UE_LOG(LogTemp, Warning, TEXT("Final bIsMoving = %s"),
	       bIsMoving ? TEXT("true") : TEXT("false"));
	if (openArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenArray is empty"));
		return;
	}

	int32 safetyCounter = 0;
	constexpr int32 maxSafetyCount = 1000;

	while (openArray.Num() > 0 && safetyCounter++ < maxSafetyCount)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("BuildPath: pathArray.Num = %d, moveRange = %d"),
		       pathArray.Num(), moveRange);
		UE_LOG(LogTemp, Warning, TEXT("Final bIsMoving = %s"),
		       bIsMoving ? TEXT("true") : TEXT("false"));
		if (safetyCounter > maxSafetyCount)
		{
			UE_LOG(LogTemp, Warning, TEXT("Path Safe Break"));
			break;
		}
		currentTile = openArray[0];
		openArray.RemoveAt(0);

		if (!IsValid(currentTile))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s currentTile PathFind Error"),
			       *GetName());
			return;
		}
		if (!IsValid(goalTile))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s goalTile PathFind Error"),
			       *GetName());
			return;
		}
		// 목표 도달했으면 종료
		if (currentTile == goalTile)
		{
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
}

void ABaseBattlePawn::AddOpenByOffset(FIntPoint offset)
{
	FIntPoint nextCoord = currentTile->gridCoord + offset;
	AGridTileManager* tileManger = Cast<AGridTileManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), TileManagerFactory));

	if (AGridTile* tile = tileManger->map.FindRef(nextCoord))
	{
		float previousCost = tile->tCostValue;
		tile->SetCost(currentTile, goalTile);

		// if (!tile->parentTile || tile->tCostValue < previousCost)
		// {
		// 	tile->parentTile = currentTile;
		if (tile != currentTile && (!tile->parentTile || tile->tCostValue <
			previousCost))
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

	// goalTile 또는 goalTile->parentTile 자체가 nullptr일 수 있다.
	if (!goalTile || !goalTile->parentTile)
	{
		UE_LOG(LogTemp, Error,
		       TEXT("BuildPath aborted: goalTile or parent is null"));
		OnMoveEnd();
		return;
	}
	if (startTile == goalTile)
	{
		UE_LOG(LogTemp, Warning, TEXT("Start tile is same as goal tile"));
		OnMoveEnd();
		return;
	}

	TSet<AGridTile*> visitePathTiles;
	// 찾은 길 표시
	AGridTile* temp = goalTile;

	while (temp && temp->parentTile)
	{
		if (visitePathTiles.Contains(temp) || temp == temp->parentTile)
		{
			UE_LOG(LogTemp, Warning,
			       TEXT(" Infinite loop detected in BuildPath"));
			break;
		}

		visitePathTiles.Add(temp);
		pathArray.Insert(temp, 0); // 역방향으로 삽입
		temp = temp->parentTile;
	}

	if (pathArray.Num() == 0)
	{
		OnMoveEnd(); // 이동 실패 시 턴 종료
		return;
	}

	// 경로 저장 완료했으면 이동 시작
	if (pathArray.Num() > moveRange)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("Path too long! moveRange = %d, path length = %d"),
		       moveRange, pathArray.Num());
		pathArray.SetNum(moveRange); // 최대 이동 가능 거리만큼 잘라 이동
	}

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

	if (bool isHit = GetWorld()->LineTraceSingleByChannel(
		hitInfo, start, end, ECC_Visibility, params))
	{
		AGridTile* tile = Cast<AGridTile>(hitInfo.GetActor());
		// openArray, closeArray에 해당 tile이 없을 때만
		if (IsValid(tile) && !openArray.Contains(tile) && !closedArray.
			Contains(tile))
		{
			// 해당 tile에 Cost 구하기
			tile->SetCost(currentTile, goalTile);

			// openArray 값에 tile 추가 (cost 값이 작은 tile 앞쪽에)
			int32 i = 0;
			for (i = 0; i < openArray.Num(); ++i)
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

void ABaseBattlePawn::SeeMoveRange(int32 move_Range)
{
	if (!gridTileManager || currentActionMode != EActionMode::Move)
	{
		UE_LOG(LogTemp, Warning, TEXT("SeeMoveRange : !gridTileManager || currentActionMode != EActionMode::Move"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("SeeMoveRange called"));
	highlightedTiles.Empty();
	// currentTile 기준으로 이동 범위 상하좌우 moveRange 만큼 색 변경
	FIntPoint coord = gridTileManager->GetTileLoc(currentTile);
	int32 startX = coord.X;
	int32 startY = coord.Y;

	// 멘해튼 거리를 활용하여 대각선 포함 상하좌우에 있는 타일 색 변경
	for (int y = startY - move_Range; y <= startY + move_Range; y++)
	{
		for (int x = startX - move_Range; x <= startX + move_Range; x++)
		{
			// 더 넓은 정사각형 모양으로 머티리얼 수정
			// FIntPoint checkCoord(x, y);
			// // 타일이 있는지 체크
			// if (gridTileManager->IsValidTile(checkCoord))
			// {
			// 	// 있다면 그 타일에 생상 변경
			// 	AGridTile* tempTile = gridTileManager->GetTile(x, y);
			// 	gridTileManager->SetTileColor(gridTileManager->GetTile(x, y), true);
			// 	highlightedTiles.Add(tempTile);
			// }
			int dx = FMath::Abs(x - startX);
			int dy = FMath::Abs(y - startY);

			if (dx + dy <= move_Range)
			{
				// 이 타일은 이동 가능 범위 안에 있음
				AGridTile* tempTile = gridTileManager->GetTile(x, y);
				gridTileManager->SetTileColor(gridTileManager->GetTile(x, y), true);
				highlightedTiles.Add(tempTile);
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

void ABaseBattlePawn::OnMoveEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("Move Complete"));
	OnTurnEnd();
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
	currentTile = nullptr;
	startTile = nullptr;
	goalTile = nullptr;
}

void ABaseBattlePawn::BillboardBattleUnitStateUI()
{
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

	GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(
		start, dir);
	end = start + dir * 10000;

	ABaseBattlePawn* currentHovered = nullptr;

	if (GetWorld()->LineTraceSingleByChannel(hitInfo, start, end,
	                                         ECC_Visibility, params))
	{
		currentHovered = Cast<ABaseBattlePawn>(hitInfo.GetActor());
	}

	// 이전과 현재가 다르면 UI 갱신
	if (lastHoveredPawn != currentHovered)
	{
		if (lastHoveredPawn && lastHoveredPawn->battleUnitStateUI)
		{
			lastHoveredPawn->battleUnitStateUI->ShowBaseUI();
			lastHoveredPawn->battleUnitStateComp->
			                 SetDrawSize(FVector2D(50, 10));
		}

		if (currentHovered && currentHovered->battleUnitStateUI)
		{
			currentHovered->battleUnitStateUI->ShowHoverUI();
			currentHovered->battleUnitStateComp->SetDrawSize(
				FVector2D(150, 130));
		}

		lastHoveredPawn = currentHovered;
	}
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

void ABaseBattlePawn::UnitMoveRotateAttack()
{
	// AStar로 나온 방향으로 이동
	if (bIsMoving && pathArray.IsValidIndex(currentPathIndex))
	{
		FVector targetLoc = pathArray[currentPathIndex]->GetActorLocation() +
			FVector(0, 0, 80);
		FVector currentLoc = GetActorLocation();
		FVector direction = (targetLoc - currentLoc).GetSafeNormal();

		// 메시 회전 부드럽게 보간 처리
		FRotator targetRot = direction.Rotation();
		constexpr float yawOffset = -90.f;
		constexpr float interpSpeed = 8.f;

		if (ABattlePlayer* player = Cast<ABattlePlayer>(this))
		{
			if (player->meshComp)
			{
				FRotator currentRot = player->meshComp->GetRelativeRotation();
				FRotator desiredRot(0.f, targetRot.Yaw + yawOffset, 0.f);
				FRotator smoothRot = FMath::RInterpTo(
					currentRot, desiredRot, GetWorld()->GetDeltaSeconds(),
					interpSpeed);
				player->meshComp->SetRelativeRotation(smoothRot);
			}
		}
		else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(this))
		{
			if (enemy->meshComp)
			{
				FRotator currentRot = enemy->meshComp->GetRelativeRotation();
				FRotator desiredRot(0.f, targetRot.Yaw + yawOffset, 0.f);
				FRotator smoothRot = FMath::RInterpTo(
					currentRot, desiredRot, GetWorld()->GetDeltaSeconds(),
					interpSpeed);
				enemy->meshComp->SetRelativeRotation(smoothRot);
			}
		}

		// 위치 보간 이동
		FVector newLoc = FMath::VInterpConstantTo(
			currentLoc, targetLoc, GetWorld()->GetDeltaSeconds(), 500.f);
		SetActorLocation(newLoc);

		if (FVector::DistSquared(currentLoc, targetLoc) < FMath::Square(5.f))
		{
			currentPathIndex++;
			if (!pathArray.IsValidIndex(currentPathIndex))
			{
				currentActionMode = EActionMode::None;
				if (auto* player = Cast<ABattlePlayer>(this))
				{
					if (player->playerAnim)
					{
						player->playerAnim->actionMode = currentActionMode;
						UE_LOG(LogTemp, Warning,
						       TEXT("actionMode set to None"));
						
					}
				}
				else if (auto* enemy = Cast<ABaseEnemy>(this))
				{
					if (enemy->enemyAnim)
					{
						enemy->enemyAnim->actionMode = currentActionMode;
					}
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

	// 타겟 방향으로 회전
	if (bWantsToAttack && attackTarget)
	{
		FVector directionToTarget = (attackTarget->GetActorLocation() -
			GetActorLocation()).GetSafeNormal2D();
		FRotator desiredRot = directionToTarget.Rotation();
		constexpr float interpSpeed = 5.f;
		// 캐릭터의 forward 기준에 맞게 조정
		constexpr float yawOffset = -90.f;

		// 현재 유닛이 Player라면
		if (ABattlePlayer* player = Cast<ABattlePlayer>(this))
		{
			if (player->meshComp)
			{
				FRotator currentRot = player->meshComp->GetRelativeRotation();
				FRotator targetMeshRot = FRotator(
					0.f, desiredRot.Yaw + yawOffset, 0.f);
				FRotator newRot = FMath::RInterpTo(
					currentRot, targetMeshRot, GetWorld()->GetDeltaSeconds(),
					interpSpeed);
				player->meshComp->SetRelativeRotation(newRot);

				// 회전이 다 됐는지 체크
				if (FMath::Abs(
						FMath::FindDeltaAngleDegrees(
							newRot.Yaw, targetMeshRot.Yaw))
					< 1.f)
				{
					bWantsToAttack = false;

					if (player->bStartMontage)
					{
						// 회전 끝나고 몽타주 실행
						player->playerAnim->PlayBaseAttackAnimation(
							TEXT("StartBaseAttack"));
						player->bStartMontage = false;
						UE_LOG(LogTemp, Warning, TEXT("Play BaseAttack"));

						// 어떤 스킬 사용했는지
						if (player->curSkillName == "")
						{
							return;
						}
						player->battleUnitStateComp->SetDrawSize(
							FVector2D(150, 100));
						player->battleUnitStateUI->SetPrintSkillName(
							player->curSkillName);
						player->battleUnitStateUI->ShowPrintSkillNameUI();

						FTimerHandle skillNameUIHandle;
						GetWorld()->GetTimerManager().ClearTimer(
							skillNameUIHandle);
						GetWorld()->GetTimerManager().SetTimer(
							skillNameUIHandle, [player]()
							{
								if (player && player->battleUnitStateUI)
								{
									player->battleUnitStateUI->ShowBaseUI();
									player->battleUnitStateComp->SetDrawSize(
										FVector2D(50, 10));
								}
							}, 2.0f, false);
					}
				}
			}
		}
		else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(this))
		{
			if (enemy->meshComp)
			{
				FRotator currentRot = enemy->meshComp->GetRelativeRotation();
				FRotator targetMeshRot = FRotator(
					0.f, desiredRot.Yaw + yawOffset, 0.f);
				FRotator newRot = FMath::RInterpTo(
					currentRot, targetMeshRot, GetWorld()->GetDeltaSeconds(),
					interpSpeed);
				enemy->meshComp->SetRelativeRotation(newRot);

				if (FMath::Abs(
						FMath::FindDeltaAngleDegrees(
							newRot.Yaw, targetMeshRot.Yaw))
					< 1.f)
				{
					bWantsToAttack = false;
					if (enemy->bStartMontage)
					{
						// 회전 끝나고 몽타주 실행
						if (enemy->enemyAnim)
						{
							enemy->enemyAnim->PlayBaseAttackAnimation(
								TEXT("StartBaseAttack"));
							UE_LOG(LogTemp, Warning,
							       TEXT("EnemyBaseAttackAnimation"));

							// 어떤 스킬 사용했는지
							if (enemy->curSkillName == "")
							{
								return;
							}
							enemy->battleUnitStateComp->SetDrawSize(
								FVector2D(150, 100));
							enemy->battleUnitStateUI->SetPrintSkillName(
								enemy->curSkillName);
							enemy->battleUnitStateUI->ShowPrintSkillNameUI();

							FTimerHandle skillNameUIHandle;
							GetWorld()->GetTimerManager().ClearTimer(
								skillNameUIHandle);
							GetWorld()->GetTimerManager().SetTimer(
								skillNameUIHandle, [enemy]()
								{
									if (enemy && enemy->battleUnitStateUI)
									{
										enemy->battleUnitStateUI->ShowBaseUI();
										enemy->battleUnitStateComp->SetDrawSize(
											FVector2D(50, 10));
									}
								}, 2.0f, false);
						}
						else
						{
							UE_LOG(LogTemp, Error,
							       TEXT("enemy->enemyAnim is nullptr!"));
						}
						enemy->bStartMontage = false;
						UE_LOG(LogTemp, Warning, TEXT("Play BaseAttack"));

						// 끝났으면 턴 종료 처리
						// OnTurnEnd();
					}
				}
			}
		}
	}
}

void ABaseBattlePawn::InitEnemyState()
{
	if (!(battleUnitStateUI && phaseManager))
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("InitEnemyState : !(battleUnitStateUI && phaseManager"));
		return;
	}

	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(this))
	{
		// 처음 값 세팅
		enemy->enemybattleState = NewObject<UEnemyBattleState>(this);

		// 애너미 랜덤 성격 세팅
		InitTraits();
		
		for (const FString trait : enemy->enemybattleState->traits)
		{
			UE_LOG(LogTemp, Warning, TEXT("Trait : %s"), *trait);
		}
		// 기본 스텟 설정(일단 오우거로)
		enemy->enemybattleState->hp = 90;
		enemy->enemybattleState->attack = 10;
		enemy->enemybattleState->defense = 5;
		enemy->enemybattleState->resistance = 2;
		enemy->enemybattleState->move_Range = 3;
		enemy->enemybattleState->critical_Rate = 1.0f;
		enemy->enemybattleState->critical_Damage = 1.5f;
		enemy->enemybattleState->speed = 3;

		// 성격에 따른 값 추가 세팅
		ApplyTraitModifiers(enemy->enemybattleState);
		
		// enemy에 moveRange 세팅 작업
		enemy->moveRange = FMath::Max(1, enemy->enemybattleState->move_Range);

		
		// 전송용 구조체에 값 세팅
		enemy->enemybattleState->enemyStatus.hp = enemybattleState->hp;
		enemy->enemybattleState->enemyStatus.attack = enemybattleState->attack;
		enemy->enemybattleState->enemyStatus.defense = enemybattleState->defense;
		enemy->enemybattleState->enemyStatus.resistance = enemybattleState->resistance;
		enemy->enemybattleState->enemyStatus.move_Range = enemybattleState->move_Range;
		enemy->enemybattleState->enemyStatus.critical_Rate = enemybattleState->critical_Rate;
		enemy->enemybattleState->enemyStatus.critical_Damage = enemybattleState->critical_Damage;
		enemy->enemybattleState->enemyStatus.speed = enemybattleState->speed;

		
		UE_LOG(LogTemp, Warning,TEXT("Enemy State Set hp : %d, atk : %d, def : %d, res : %d, mov : %d, "", crit : %f, crit_Damge : %f, spd : %d"),
			   enemy->enemybattleState->hp,
			   enemy->enemybattleState->attack,
			   enemy->enemybattleState->defense,
			   enemy->enemybattleState->resistance,
			   enemy->enemybattleState->move_Range,
			   enemy->enemybattleState->critical_Rate,
			   enemy->enemybattleState->critical_Damage,
			   enemy->enemybattleState->speed);
		UE_LOG(LogTemp, Warning,TEXT("API Enemy State Set hp : %d, atk : %d, def : %d, res : %d, mov : %d, "", crit : %f, crit_Damge : %f, spd : %d"),
				enemy->enemybattleState->enemyStatus.hp,
				enemy->enemybattleState->enemyStatus.attack,
				enemy->enemybattleState->enemyStatus.defense,
				enemy->enemybattleState->enemyStatus.resistance,
				enemy->enemybattleState->enemyStatus.move_Range,
				enemy->enemybattleState->enemyStatus.critical_Rate,
				enemy->enemybattleState->enemyStatus.critical_Damage,
				enemy->enemybattleState->enemyStatus.speed);
		
		FString name = "";
		name = FString::Printf(TEXT("Enemy%d"), phaseManager->unitEnemyNameindex);
		phaseManager->unitEnemyNameindex++;

		enemy->battleUnitStateUI->SetUnitName(name);
		enemy->battleUnitStateUI->SetHPUI(enemy);
	}
}

void ABaseBattlePawn::InitTraits()
{
	if (auto* enemy = Cast<ABaseEnemy>(this))
	{
		for (const FString trait : enemyTraits)
		{
			int32 seletTrait = FMath::RandRange(0, 19);
			if (seletTrait <= 3 && !enemy->enemybattleState->traits.Contains(trait))
			{
				enemy->enemybattleState->traits.Add(trait);
			}
			// 성격 개수가 3개라면 return
			if (enemy->enemybattleState->traits.Num() >= 3) return;
		}
		// 개수가 3개보다 작으면 다시 Init을 실행
		if (enemy->enemybattleState->traits.Num() < 3)
		{
			InitTraits();
		}
	}
}

void ABaseBattlePawn::ApplyTraitModifiers(UEnemyBattleState* state)
{
	for (const FString& trait : state->traits)
	{
		if (trait == TEXT("강인함"))
		{
			state->hp *= 1.3f;
			state->attack *= 0.9f;
		}
		else if (trait == TEXT("잔잔함"))
		{
			state->hp *= 1.3f;
			state->move_Range -= 1;
		}
		else if (trait == TEXT("호전적"))
		{
			state->attack *= 1.2f;
			state->critical_Rate *= 0.85f;
		}
		else if (trait == TEXT("충동적"))
		{
			state->attack *= 1.2f;
			state->defense *= 0.9f;
		}
		else if (trait == TEXT("수비적"))
		{
			state->defense *= 1.2f;
			state->speed *= 0.9f;
		}
		else if (trait == TEXT("신중함"))
		{
			state->defense *= 1.2f;
			state->critical_Rate *= 0.85f;
		}
		else if (trait == TEXT("관찰꾼"))
		{
			state->critical_Rate *= 1.3f;
			state->attack *= 0.9f;
		}
		else if (trait == TEXT("잔인함"))
		{
			state->critical_Rate *= 1.3f;
			state->speed *= 0.9f;
		}
		else if (trait == TEXT("겁쟁이"))
		{
			state->move_Range += 2;
			state->attack *= 0.9f;
		}
		else if (trait == TEXT("허세꾼"))
		{
			state->move_Range += 2;
			state->defense *= 0.9f;
		}
		else if (trait == TEXT("교란꾼"))
		{
			state->speed *= 1.2f;
			state->attack *= 0.9f;
		}
		else if (trait == TEXT("파괴적"))
		{
			state->speed *= 1.2f;
			state->defense *= 0.9f;
		}
		else if (trait == TEXT("협동적"))
		{
			state->hp *= 1.15f;
			state->defense *= 1.1f;
			state->speed *= 0.9f;
		}
		else if (trait == TEXT("용감함"))
		{
			state->attack *= 1.1f;
			state->defense *= 1.1f;
			state->critical_Rate *= 0.85f;
		}
		else if (trait == TEXT("조화적"))
		{
			state->attack *= 1.1f;
			state->defense *= 1.1f;
			state->speed *= 0.9f;
		}
		else if (trait == TEXT("고립적"))
		{
			state->attack *= 1.1f;
			state->speed *= 1.1f;
			state->defense *= 0.9f;
		}
		else if (trait == TEXT("지능적"))
		{
			state->defense *= 1.1f;
			state->critical_Rate *= 1.15f;
			state->move_Range -= 1;
		}
		else if (trait == TEXT("냉정함"))
		{
			state->defense *= 1.1f;
			state->critical_Rate *= 1.15f;
			state->speed *= 0.9f;
		}
		else if (trait == TEXT("원한꾼"))
		{
			state->critical_Rate *= 1.15f;
			state->speed *= 1.1f;
			state->defense *= 0.9f;
		}
		else if (trait == TEXT("민첩함"))
		{
			state->move_Range += 1;
			state->speed *= 1.1f;
			state->defense *= 0.9f;
		}
	}
}

void ABaseBattlePawn::InitOtherClassPointer()
{
	// PhaseManager
	phaseManager = Cast<AUPhaseManager>(GetWorld()->GetGameState());
	if (phaseManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("phaseManager is Set"));
	}
	// TurnManager
	turnManager = Cast<ATurnManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), turnManagerFactory));
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
	gridTileManager = Cast<AGridTileManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), TileManagerFactory));
	if (gridTileManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("gridTileManager is Set"));
	}
	// BaseHP UI 보여주기
	battleUnitStateUI = Cast<UBattleUnitStateUI>(
		this->battleUnitStateComp->GetWidget());
	if (battleUnitStateUI) UE_LOG(LogTemp, Warning,
	                              TEXT("battleUnitStateUI is Set"));
}

void ABaseBattlePawn::InitAPUI()
{
	if (!(pc && hud && hud->mainUI && hud->mainUI->HB_AP))
	{
		UE_LOG(LogTemp, Warning,
		       TEXT(
			       "InitAPUI is Not Set : pc && hud && hud->mainUI && hud->mainUI->HB_AP"
		       ));
		return;
	}

	// AP UI 초기화
	hud->mainUI->HB_AP->ClearChildren();

	// 현재 턴 유닛이 Player인 경우에만
	if (ABattlePlayer* player = Cast<ABattlePlayer>(this))
	{
		for (int32 i = 0; i < player->curAP; ++i)
		{
			hud->mainUI->AddAP(); // AP UI 하나씩 추가
		}
	}
}
