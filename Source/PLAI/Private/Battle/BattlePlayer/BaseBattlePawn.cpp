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
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
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
	cameraComp->SetRelativeLocationAndRotation(FVector(-600, 0, 1000), FRotator(-60, 0, 0));
	
	// 이름, HP, Armor 설정 컴포넌트
	battleUnitStateComp = CreateDefaultSubobject<UWidgetComponent>("BattleUnitStateComp");
	battleUnitStateComp->SetupAttachment(RootComponent);
	battleUnitStateComp->SetRelativeLocationAndRotation(FVector(0, 0, 170), FRotator(0));
	battleUnitStateComp->SetGenerateOverlapEvents(false); // 충돌 막기
	battleUnitStateComp->SetVisibility(true); 
	battleUnitStateComp->SetCastShadow(false);

	ConstructorHelpers::FClassFinder<UBattleUnitStateUI> tempBattleUnitStateUI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/JS/UI/WBP_BattleUnitState.WBP_BattleUnitState_C'"));
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

	turnManager = Cast<ATurnManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), turnManagerFactory));

	// AP 세팅
	SetAP();

	// BaseHP UI 보여주기
	if (UBattleUnitStateUI* ui = Cast<UBattleUnitStateUI>(this->battleUnitStateComp->GetWidget()))
	{
		battleUnitStateComp->SetDrawSize(FVector2D(50, 10));
		ui->ShowBaseUI();
	}
}

// Called every frame
void ABaseBattlePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Hover UI 띄우기 위한 함수
	OnMouseHover();

	// AStar로 나온 방향으로 이동
	if (bIsMoving && pathArray.IsValidIndex(currentPathIndex))
	{
		FVector targetLoc = pathArray[currentPathIndex]->GetActorLocation() + FVector(0, 0, 80);
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
				FRotator smoothRot = FMath::RInterpTo(currentRot, desiredRot, DeltaTime, interpSpeed);
				player->meshComp->SetRelativeRotation(smoothRot);
			}
		}
		else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(this))
		{
			if (enemy->meshComp)
			{
				FRotator currentRot = enemy->meshComp->GetRelativeRotation();
				FRotator desiredRot(0.f, targetRot.Yaw + yawOffset, 0.f);
				FRotator smoothRot = FMath::RInterpTo(currentRot, desiredRot, DeltaTime, interpSpeed);
				enemy->meshComp->SetRelativeRotation(smoothRot);
			}
		}

		// 위치 보간 이동
		FVector newLoc = FMath::VInterpConstantTo(currentLoc, targetLoc, DeltaTime, 500.f);
		SetActorLocation(newLoc);

		if (FVector::DistSquared(currentLoc, targetLoc) < FMath::Square(5.f))
		{
			currentPathIndex++;
			if (!pathArray.IsValidIndex(currentPathIndex))
			{
				currentActionMode = EActionMode::None;
				if (auto* player = Cast<ABattlePlayer>(this))
				{
					if (auto* anim = Cast<UBattlePlayerAnimInstance>(player->meshComp->GetAnimInstance()))
					{
						anim->actionMode = currentActionMode;
						UE_LOG(LogTemp, Warning, TEXT("actionMode set to None"));
					}
				}
				else if (auto* enemy = Cast<ABaseEnemy>(this))
				{
					if (auto* anim = Cast<UBattleEnemyAnimInstance>(enemy->meshComp->GetAnimInstance()))
					{
						anim->actionMode = currentActionMode;
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
		FVector directionToTarget = (attackTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
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
				FRotator targetMeshRot = FRotator(0.f, desiredRot.Yaw + yawOffset, 0.f);
				FRotator newRot = FMath::RInterpTo(currentRot, targetMeshRot, DeltaTime, interpSpeed);
				player->meshComp->SetRelativeRotation(newRot);

				// 회전이 다 됐는지 체크
				if (FMath::Abs(FMath::FindDeltaAngleDegrees(newRot.Yaw, targetMeshRot.Yaw)) < 1.f)
				{
					bWantsToAttack = false;

					if (player->bStartMontage)
					{
						// 회전 끝나고 몽타주 실행
						player->playerAnim->PlayBaseAttackAnimation(TEXT("StartBaseAttack"));
						player->bStartMontage = false;
						UE_LOG(LogTemp, Warning, TEXT("Play BaseAttack"));
					}
					
				}
			}
		}
		else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(this))
		{
			if (enemy->meshComp)
			{
				FRotator currentRot = enemy->meshComp->GetRelativeRotation();
				FRotator targetMeshRot = FRotator(0.f, desiredRot.Yaw + yawOffset, 0.f);
				FRotator newRot = FMath::RInterpTo(currentRot, targetMeshRot, DeltaTime, interpSpeed);
				enemy->meshComp->SetRelativeRotation(newRot);

				if (FMath::Abs(FMath::FindDeltaAngleDegrees(newRot.Yaw, targetMeshRot.Yaw)) < 1.f)
				{
					bWantsToAttack = false;
					if (enemy->bStartMontage)
					{
						// 회전 끝나고 몽타주 실행
						if (enemy->enemyAnim)
						{
							enemy->enemyAnim->PlayBaseAttackAnimation(TEXT("StartBaseAttack"));
							UE_LOG(LogTemp, Warning, TEXT("PlayBaseAttackAnimation"));
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("enemy->enemyAnim is nullptr!"));
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
	// UI 빌보드 처리
	BillboardBattleUnitStateUI();
}

void ABaseBattlePawn::OnTurnStart()
{
	// 턴 종료 초기화
	bTurnEnded = false;
	
	// 기본 공격 초기화
	bBaseAttack = true;

	// 턴이 시작됐으면 턴 카운트 1 증가
	if (turnManager)
	{
		turnManager->turnCount++;
		UE_LOG(LogTemp, Warning, TEXT("Turn Count %d"), turnManager->turnCount);

		// 턴 유닛 이름 UI에 세팅
		if (APlayerController* pc = GetWorld()->GetFirstPlayerController())
		{
			if (ABattleHUD* hud = Cast<ABattleHUD>(pc->GetHUD()))
			{
				if (hud->mainUI && hud->mainUI->WBP_CycleAndTurn)
				{
					FString s = Cast<ABattlePlayer>(turnManager->curUnit)
						            ? TEXT("Player")
						            : TEXT("Enemy");
					hud->mainUI->WBP_CycleAndTurn->SetTurnText(s);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TurnManager is nullptr"));
	}

	// BattlePlayerInfo UI 세팅
	if (auto* player = Cast<ABattlePlayer>(this))
	{
		if (APlayerController* pc = GetWorld()->GetFirstPlayerController())
		{
			if (ABattleHUD* hud = Cast<ABattleHUD>(pc->GetHUD()))
			{
				if (hud->mainUI && hud->mainUI->WBP_Player)
				{
					FString name = player->GetActorNameOrLabel();
					hud->mainUI->WBP_Player->SetPlayerName(name);
					hud->mainUI->WBP_Player->SetPlayerHPUI(player);
				}
			}
		}
	}
	
	
	// 상태이상이 있다면 대미지 및 버프, 디버프 처리하기
	ApplyStatusEffect();

	// Player라면
	if (ABattlePlayer* player = Cast<ABattlePlayer>(this))
	{
		player->GetAP();
		UE_LOG(LogTemp, Warning, TEXT("%s -> curAP : %d"), *player->GetName(), player->curAP);
	}
	// Enemy라면
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(this))
	{
		// 턴 시작 시 AP 증가
		if (!enemybattleState)
		{
			UE_LOG(LogTemp, Error, TEXT("enemybattleState is null on OnTurnStart"));
			return;
		}

		enemybattleState->curAP += enemybattleState->GetAP(enemybattleState->curAP);
		UE_LOG(LogTemp, Warning, TEXT("Current AP: %d"), enemybattleState->curAP);

		UE_LOG(LogTemp, Warning, TEXT("BaseBattlePawn::OnTurnStart"));
		ABaseBattlePawn* CapturedUnit = this;

		FTimerHandle battleAPIHandle;
		GetWorld()->GetTimerManager().SetTimer(battleAPIHandle,FTimerDelegate::CreateLambda([this, CapturedUnit]()
       {
           UE_LOG(LogTemp, Warning,TEXT("BaseBattlePawn::In Lambda"));
           if (turnManager && turnManager->phaseManager)
           {
               UE_LOG(LogTemp,Warning,TEXT("BaseBattlePawn::turnManager, phaseManager is Set"));turnManager->phaseManager->TrySendbattleState(CapturedUnit);
           }
       }), 1.0f, false);
	}
	UE_LOG(LogTemp, Warning, TEXT("%s Turn Start"), *GetName());
}

void ABaseBattlePawn::OnTurnEnd()
{
	// 여러 번 호출 방지
	if (bTurnEnded) return;
	bTurnEnded = true;
	
	UE_LOG(LogTemp, Warning, TEXT("%s Turn End"), *GetName());
	// 입력 막고 FSM 종료

	// Casting을 통해 현재 유닛이 player 또는 enemy라면 그쪽 함수 실행
	if (auto* phaseManager = Cast<AUPhaseManager>(GetWorld()->GetGameState()))
	{
		// 턴 종료 상태라면 return
		if (phaseManager->turnManager->curTurnState == ETurnState::TurnEnd)
		{
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
}


void ABaseBattlePawn::OnMouseLeftClick()
{
	FVector start, end, dir;
	FHitResult hitInfo;
	FCollisionQueryParams params;

	GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(
		start, dir);
	end = start + dir * 10000;

	if (GetWorld()->LineTraceSingleByChannel(hitInfo, start, end,ECC_Visibility, params))
	{
		// 상대와 거리 측정
		float dist = FVector::Dist(GetActorLocation(),
		                           hitInfo.GetActor()->GetActorLocation());

		// 자신의 moveRange * 100 보다 작거나 같을 때
		if (dist > (this->battlePlayerState->playerStatus.move_Range * 100))
		{
			UE_LOG(LogTemp, Warning, TEXT("Test Click : Dist too be far"));
			return;
		}
		
		EActionMode curSkillState = currentActionMode;
		UE_LOG(LogTemp, Warning, TEXT("Test Click : Current Action Mode %s"),
		       *UEnum::GetValueAsString(curSkillState));
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
			if (auto* anim = Cast<UBattlePlayerAnimInstance>(player->meshComp->GetAnimInstance()))
			{
				anim->actionMode = currentActionMode;
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
			if (auto* anim = Cast<UBattlePlayerAnimInstance>(player->meshComp->GetAnimInstance()))
			{
				anim->actionMode = currentActionMode;
			}
			player->bStartMontage = true;
		}
		
	}
	// 공격 대상이 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		// 타겟 저장
		targetPlayer = player;
		bWantsToAttack = true;
		attackTarget = targetPlayer;
		
		if (auto* anim = Cast<UBattleEnemyAnimInstance>(enemy->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
		enemy->bStartMontage = true;
	}
}

void ABaseBattlePawn::PlayerParalysis(FHitResult& hitInfo)
{
	if (auto* player = Cast<ABattlePlayer>(this))
	{
		if (auto* anim = Cast<UBattlePlayerAnimInstance>(player->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
	else if (auto* enemy = Cast<ABaseEnemy>(this))
	{
		if (auto* anim = Cast<UBattleEnemyAnimInstance>(enemy->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("PlayerParalysis In"));
	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerParalysis In Enemy"));
		ApplyAttack(enemy, EActionMode::Paralysis);
	}
	// 공격 대상이 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerParalysis In Player"));
		ApplyAttack(player, EActionMode::Paralysis);
	}
}

void ABaseBattlePawn::PlayerPoison(FHitResult& hitInfo)
{
	if (auto* player = Cast<ABattlePlayer>(this))
	{
		if (auto* anim = Cast<UBattlePlayerAnimInstance>(player->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
	else if (auto* enemy = Cast<ABaseEnemy>(this))
	{
		if (auto* anim = Cast<UBattleEnemyAnimInstance>(enemy->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		ApplyAttack(enemy, EActionMode::Poison);
	}
	// 공격 대상이 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		ApplyAttack(player, EActionMode::Poison);
	}
}

void ABaseBattlePawn::PlayerVulnerable(FHitResult& hitInfo)
{
	if (auto* player = Cast<ABattlePlayer>(this))
	{
		if (auto* anim = Cast<UBattlePlayerAnimInstance>(player->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
	else if (auto* enemy = Cast<ABaseEnemy>(this))
	{
		if (auto* anim = Cast<UBattleEnemyAnimInstance>(enemy->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		ApplyAttack(enemy, EActionMode::Vulnerable);
	}
	// 공격 대상이 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		ApplyAttack(player, EActionMode::Vulnerable);
	}
}

void ABaseBattlePawn::PlayerWeaking(FHitResult& hitInfo)
{
	if (auto* player = Cast<ABattlePlayer>(this))
	{
		if (auto* anim = Cast<UBattlePlayerAnimInstance>(player->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
	else if (auto* enemy = Cast<ABaseEnemy>(this))
	{
		if (auto* anim = Cast<UBattleEnemyAnimInstance>(enemy->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
	
	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		ApplyAttack(enemy, EActionMode::Weakening);
	}
	// 공격 대상이 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		ApplyAttack(player, EActionMode::Weakening);
	}
}

void ABaseBattlePawn::PlayerFatal(FHitResult& hitInfo)
{
	if (auto* player = Cast<ABattlePlayer>(this))
	{
		if (auto* anim = Cast<UBattlePlayerAnimInstance>(player->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
	else if (auto* enemy = Cast<ABaseEnemy>(this))
	{
		if (auto* anim = Cast<UBattleEnemyAnimInstance>(enemy->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		ApplyAttack(enemy, EActionMode::Fatal);
	}
	// 공격 대상이 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		ApplyAttack(player, EActionMode::Fatal);
	}
}

void ABaseBattlePawn::PlayerRupture(FHitResult& hitInfo)
{
	if (auto* player = Cast<ABattlePlayer>(this))
	{
		if (auto* anim = Cast<UBattlePlayerAnimInstance>(player->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
	else if (auto* enemy = Cast<ABaseEnemy>(this))
	{
		if (auto* anim = Cast<UBattleEnemyAnimInstance>(enemy->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		ApplyAttack(enemy, EActionMode::Rupture);
	}
	// 공격 대상이 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		ApplyAttack(player, EActionMode::Rupture);
	}
}

void ABaseBattlePawn::PlayerRoar(FHitResult& hitInfo)
{
	if (auto* player = Cast<ABattlePlayer>(this))
	{
		if (auto* anim = Cast<UBattlePlayerAnimInstance>(player->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
	else if (auto* enemy = Cast<ABaseEnemy>(this))
	{
		if (auto* anim = Cast<UBattleEnemyAnimInstance>(enemy->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		ApplyAttack(enemy, EActionMode::Roar);
	}
	// 공격 대상이 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		ApplyAttack(player, EActionMode::Roar);
	}
}

void ABaseBattlePawn::PlayerBattleCry(FHitResult& hitInfo)
{
	if (auto* player = Cast<ABattlePlayer>(this))
	{
		if (auto* anim = Cast<UBattlePlayerAnimInstance>(player->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
	else if (auto* enemy = Cast<ABaseEnemy>(this))
	{
		if (auto* anim = Cast<UBattleEnemyAnimInstance>(enemy->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
	// 공격 대상이 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		ApplyAttack(enemy, EActionMode::BattleCry);
	}
	// 공격 대상이 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		ApplyAttack(player, EActionMode::BattleCry);
	}
}

void ABaseBattlePawn::InitEnemyState()
{
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(this))
	{
		// 처음 값 세팅
		enemybattleState = NewObject<UEnemyBattleState>(this);
		enemybattleState->hp = 80;
		
		enemybattleState->attack = 9;
		enemybattleState->defense = 6;
		enemybattleState->resistance = 3;
		enemybattleState->move_Range = 5;
		enemybattleState->critical_Rate = 0.04f;
		enemybattleState->critical_Damage = 1.5f;
		enemybattleState->speed = 4;

		// enemy에 moveRange 세팅 작업
		enemy->moveRange = enemy->enemybattleState->move_Range;
		
		// 전송용 구조체에 값 세팅
		enemybattleState->enemyStatus.hp = enemybattleState->hp;
		enemybattleState->enemyStatus.attack = enemybattleState->attack;
		enemybattleState->enemyStatus.defense = enemybattleState->defense;
		enemybattleState->enemyStatus.resistance = enemybattleState->resistance;
		enemybattleState->enemyStatus.move_Range = enemybattleState->move_Range;
		enemybattleState->enemyStatus.critical_Rate = enemybattleState->
			critical_Rate;
		enemybattleState->enemyStatus.critical_Damage = enemybattleState->
			critical_Damage;
		enemybattleState->enemyStatus.speed = enemybattleState->speed;

		if (UBattleUnitStateUI* ui = Cast<UBattleUnitStateUI>(enemy->battleUnitStateComp->GetWidget()))
		{
			FString name = "";
			if (auto* pm = Cast<AUPhaseManager>(GetWorld()->GetGameState()))
			{
				name = FString::Printf(TEXT("Enemy%d"), pm->unitEnemyNameindex);
				pm->unitEnemyNameindex++;
			}
			ui->SetUnitName(name);
			ui->SetHPUI(enemy);
		}
	}
}

void ABaseBattlePawn::GetDamage(ABaseBattlePawn* unit, int32 damage)
{
	// player HP가 0보다 크다면
	UE_LOG(LogTemp, Warning, TEXT("In GetDamage"));
	if (ABattlePlayer* player = Cast<ABattlePlayer>(unit))
	{
		UE_LOG(LogTemp, Warning, TEXT("state && state->playerStatus.hp > 0"));
		// 피를 깎는다.
		player->battlePlayerState->playerStatus.hp = FMath::Max(0, player->battlePlayerState->playerStatus.hp - damage);
		UE_LOG(LogTemp, Warning, TEXT("Damage : %d, playerHP : %d"), damage,player->battlePlayerState->playerStatus.hp);
		// HP를 UI에 업데이트
		if (UBattleUnitStateUI* ui = Cast<UBattleUnitStateUI>(player->battleUnitStateComp->GetWidget()))
		{
			ui->UpdateHP(player->battlePlayerState->playerStatus.hp);
			
			// BattlePlayerInfo HP 업데이트
			if (APlayerController* pc = GetWorld()->GetFirstPlayerController())
			{
				if (ABattleHUD* hud = Cast<ABattleHUD>(pc->GetHUD()))
				{
					if (hud->mainUI && hud->mainUI->WBP_Player)
					{
						hud->mainUI->WBP_Player->PlayerUpdateHP(player, player->battlePlayerState->playerStatus.hp);
					}
				}
			}
		}
		// hp가 0보다 작으면 사망
		if (player->battlePlayerState->playerStatus.hp <= 0)
		{
			player->battlePlayerState->playerLifeState = ELifeState::Dead;
			UE_LOG(LogTemp, Warning, TEXT("Enemy Dead %s"),*UEnum::GetValueAsString(player->battlePlayerState->playerLifeState));
		}
	}
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(unit))
	{
		// 피를 깎는다.
		enemy->enemybattleState->hp = FMath::Max(
			0, enemy->enemybattleState->hp - damage);
		UE_LOG(LogTemp, Warning, TEXT("Damage : %d, enemyHP : %d"), damage,
		       enemy->enemybattleState->hp);
		if (UBattleUnitStateUI* ui = Cast<UBattleUnitStateUI>(enemy->battleUnitStateComp->GetWidget()))
		{
			ui->UpdateHP(enemy->enemybattleState->hp);
		}
		// hp가 0보다 작으면 사망
		if (enemy->enemybattleState->hp <= 0)
		{
			enemy->enemybattleState->enemyLifeState = ELifeState::Dead;
			UE_LOG(LogTemp, Warning, TEXT("Enemy Dead %s"),
			       *UEnum::GetValueAsString(enemy->enemybattleState->
				       enemyLifeState));
		}
	}

	// 대미지를 줬다면 현재 공격 상태를 None으로 초기화
	if (auto* player = Cast<ABattlePlayer>(this))
	{
		if (auto* anim = Cast<UBattlePlayerAnimInstance>(player->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
	else if (auto* enemy = Cast<ABaseEnemy>(this))
	{
		if (auto* anim = Cast<UBattleEnemyAnimInstance>(enemy->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
}

void ABaseBattlePawn::ApplyAttack(ABaseBattlePawn* targetUnit,
                                  EActionMode attackType)
{
	UE_LOG(LogTemp, Warning,
	       TEXT("[ApplyAttack] Called by %s with attackType %d"), *GetName(),
	       (int32)attackType);

	// (기본스탯 + 장비) * 스킬 계수 * (1 + (성격 + 상태효과))
	if (auto* player = Cast<ABattlePlayer>(targetUnit))
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy Attack Player"));
		if (auto* enemy = Cast<ABaseEnemy>(this))
		{
			UE_LOG(LogTemp, Warning, TEXT("In Cast Enemy"));

			int32 atk = enemybattleState->enemyStatus.attack;
			int32 weapon = 2;
			float critical = enemybattleState->enemyStatus.critical_Rate;
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
				if (!enemy->enemybattleState->CanConsumeAP(1))
				{
					UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
					return;
				}
				skillMultiplier = 0.8f;
				// 상대에게 상태이상 및 몇 턴동안 진행 될지 추가
				player->AddStatusEffect(EStatusEffect::Weakening, 2);
				break;
			case EActionMode::Poison:
				if (!enemy->enemybattleState->CanConsumeAP(1))
				{
					UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
					return;
				}
				player->AddStatusEffect(EStatusEffect::Poison, 3);
				break;
			case EActionMode::Vulnerable:
				if (!enemy->enemybattleState->CanConsumeAP(1))
				{
					UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
					return;
				}
				player->AddStatusEffect(EStatusEffect::Vulnerable, 2);
				break;
			case EActionMode::Weakening:
				if (!enemy->enemybattleState->CanConsumeAP(1))
				{
					UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
					return;
				}
				player->AddStatusEffect(EStatusEffect::Weakening, 2);
				break;
			case EActionMode::Fatal:
				if (!enemy->enemybattleState->CanConsumeAP(2))
				{
					UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
					return;
				}
				skillMultiplier = 1.5f;
				player->AddStatusEffect(EStatusEffect::Bleeding, 2);
				break;
			case EActionMode::Rupture:
				if (!enemy->enemybattleState->CanConsumeAP(2))
				{
					UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
					return;
				}
				skillMultiplier = 1.5f;
				player->AddStatusEffect(EStatusEffect::Vulnerable, 1);
				break;
			case EActionMode::Roar:
				if (!enemy->enemybattleState->CanConsumeAP(1))
				{
					UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
					return;
				}
				player->AddStatusEffect(EStatusEffect::Weakening, 2);
				break;
			case EActionMode::BattleCry:
				if (!enemy->enemybattleState->CanConsumeAP(1))
				{
					UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
					return;
				}
				// 자신과 동료들에게 버프 부여
				// 주위 동료들에게 피해 증가 버프 부여
				// 작성해야함 레벨에 있는 unit들 다 모아서 enemy일 때 반복문으로 버프주면 될듯
				enemy->AddStatusEffect(EStatusEffect::Angry, 1);
				break;
			default:
				// 기본 타격 스킬 배율
				skillMultiplier = 1.0f;
				break;
			}

			// Rour와 BattleCry는 비공격스킬 및 버프스킬이므로 return 시키면 될듯?
			if (attackType == EActionMode::Roar || attackType ==
				EActionMode::BattleCry)
			{
				return;
			}

			// 반올림 처리
			int32 chance = FMath::RoundToInt(critical * 100.0f);
			int32 roll = FMath::RandRange(1, 100);


			if (bool bIsCrit = roll <= chance)
			{
				int32 critical_Damage = enemybattleState->enemyStatus.
					critical_Damage;
				damage = ((atk + weapon) * skillMultiplier * (1 + (personality +
					status_effect))) * critical_Damage;
				UE_LOG(LogTemp, Warning, TEXT("Critical Damage : %d"), damage);
			}
			else
			{
				damage = (atk + weapon) * skillMultiplier * (1 + (personality +
					status_effect));
				UE_LOG(LogTemp, Warning, TEXT("Damage : %d"), damage);
			}
			
			// 대미지 입히기
			GetDamage(player, damage);
		}
	}
	else if (auto* enemy = Cast<ABaseEnemy>(targetUnit))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Attack Enemy"));
		if (auto* baseAttackPlayer = Cast<ABattlePlayer>(this))
		{
			UE_LOG(LogTemp, Warning, TEXT("In Cast baseAttackPlayer"));

			int32 atk = battlePlayerState->playerStatus.attack;
			int32 weapon = 2;
			float critical = battlePlayerState->playerStatus.critical_Rate;
			int32 personality = 0;
			int32 status_effect = 0;
			int32 damage = 0;


			// 스킬 계수 및 추가 효과
			float skillMultiplier = 1.0f;

			switch (attackType)
			{
			case EActionMode::Paralysis:
				// 현재 Ap가 cost보다 크다면 실행
				if (!baseAttackPlayer->CanConsumeAP(1))
				{
					UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
					return;
				}
				skillMultiplier = 0.8f;
				// 상대에게 상태이상 및 몇 턴동안 진행 될지 추가
				enemy->AddStatusEffect(EStatusEffect::Weakening, 2);
				break;
			case EActionMode::Poison:
				// 현재 Ap가 cost보다 크다면 실행
				if (!baseAttackPlayer->CanConsumeAP(1))
				{
					UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
					return;
				}
				enemy->AddStatusEffect(EStatusEffect::Poison, 3);
				break;
			case EActionMode::Vulnerable:
				// 현재 Ap가 cost보다 크다면 실행
				if (!baseAttackPlayer->CanConsumeAP(1))
				{
					UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
					return;
				}
				enemy->AddStatusEffect(EStatusEffect::Vulnerable, 2);
				break;
			case EActionMode::Weakening:
				// 현재 Ap가 cost보다 크다면 실행
				if (!baseAttackPlayer->CanConsumeAP(1))
				{
					UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
					return;
				}
				enemy->AddStatusEffect(EStatusEffect::Weakening, 2);
				break;
			case EActionMode::Fatal:
				// 현재 Ap가 cost보다 크다면 실행
				if (!baseAttackPlayer->CanConsumeAP(2))
				{
					UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
					return;
				}
				skillMultiplier = 1.5f;
				enemy->AddStatusEffect(EStatusEffect::Bleeding, 2);
				break;
			case EActionMode::Rupture:
				// 현재 Ap가 cost보다 크다면 실행
				if (!baseAttackPlayer->CanConsumeAP(2))
				{
					UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
					return;
				}
				skillMultiplier = 1.5f;
				enemy->AddStatusEffect(EStatusEffect::Vulnerable, 1);
				break;
			case EActionMode::Roar:
				// 현재 Ap가 cost보다 크다면 실행
				if (!baseAttackPlayer->CanConsumeAP(1))
				{
					UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
					return;
				}
				enemy->AddStatusEffect(EStatusEffect::Weakening, 2);
				break;
			case EActionMode::BattleCry:
				// 현재 Ap가 cost보다 크다면 실행
				if (!baseAttackPlayer->CanConsumeAP(2))
				{
					UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
					return;
				}
				// 자신 및 주위 동료들에게 피해 증가 버프 부여
				// 작성해야함 레벨에 있는 unit들 다 모아서 enemy일 때 반복문으로 버프주면 될듯
				baseAttackPlayer->AddStatusEffect(EStatusEffect::Angry, 1);
				break;
			default:
				// 기본 타격 스킬 배율 
				skillMultiplier = 1.0f;
				break;
			}

			// Rour와 BattleCry는 비공격스킬 및 버프스킬이므로 return 시키면 될듯?
			if (attackType == EActionMode::Roar || attackType ==
				EActionMode::BattleCry)
			{
				return;
			}

			// 반올림 처리
			int32 chance = FMath::RoundToInt(critical * 100.0f);
			int32 roll = FMath::RandRange(1, 100);


			if (bool bIsCrit = roll <= chance)
			{
				int32 critical_Damage = battlePlayerState->playerStatus.
					critical_Damage;
				damage = ((atk + weapon) * skillMultiplier * (1 + (personality +
					status_effect))) * critical_Damage;
				UE_LOG(LogTemp, Warning, TEXT("Critical Damage : %d"), damage);
			}
			else
			{
				damage = (atk + weapon) * skillMultiplier * (1 + (personality +
					status_effect));
				UE_LOG(LogTemp, Warning, TEXT("Damage : %d"), damage);
			}
			
			GetDamage(enemy, damage);
		}
	}

	// 대미지를 줬다면 현재 공격 상태를 None으로 초기화
	currentActionMode = EActionMode::None;
	if (auto* player = Cast<ABattlePlayer>(this))
	{
		if (auto* anim = Cast<UBattlePlayerAnimInstance>(player->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
	else if (auto* enemy = Cast<ABaseEnemy>(this))
	{
		if (auto* anim = Cast<UBattleEnemyAnimInstance>(enemy->meshComp->GetAnimInstance()))
		{
			anim->actionMode = currentActionMode;
		}
	}
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
		if (enumPtr)
		{
			UE_LOG(LogTemp, Warning,
			       TEXT("In AddStatusEffect : %s, Duration : %d"),
			       *enumPtr->GetNameStringByValue(static_cast<int64>(newEffect)
			       ),
			       activeStatusEffects[newEffect]);
		}
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
		HandleStateusEffect(effect);

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

void ABaseBattlePawn::HandleStateusEffect(EStatusEffect effect)
{
	switch (effect)
	{
	case EStatusEffect::Poison:
		if (auto* player = Cast<ABattlePlayer>(this))
		{
			player->battlePlayerState->playerStatus.hp -= 5;
			if (UBattleUnitStateUI* ui = Cast<UBattleUnitStateUI>(player->battleUnitStateComp->GetWidget()))
			{
				ui->UpdateHP(player->battlePlayerState->playerStatus.hp);
			}
			// BattlePlayerInfo UI 세팅
			if (APlayerController* pc = GetWorld()->GetFirstPlayerController())
			{
				if (ABattleHUD* hud = Cast<ABattleHUD>(pc->GetHUD()))
				{
					if (hud->mainUI && hud->mainUI->WBP_Player)
					{
						hud->mainUI->WBP_Player->SetPlayerHPUI(player);
					}
				}
			}
		}
		else if (auto* enemy = Cast<ABaseEnemy>(this))
		{
			enemy->enemybattleState->hp -= 5;
			if (UBattleUnitStateUI* ui = Cast<UBattleUnitStateUI>(enemy->battleUnitStateComp->GetWidget()))
			{
				ui->UpdateHP(enemy->enemybattleState->hp);
			}
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
			if (UBattleUnitStateUI* ui = Cast<UBattleUnitStateUI>(player->battleUnitStateComp->GetWidget()))
			{
				ui->UpdateHP(player->battlePlayerState->playerStatus.hp);
			}
			// BattlePlayerInfo UI 세팅
			if (APlayerController* pc = GetWorld()->GetFirstPlayerController())
			{
				if (ABattleHUD* hud = Cast<ABattleHUD>(pc->GetHUD()))
				{
					if (hud->mainUI && hud->mainUI->WBP_Player)
					{
						hud->mainUI->WBP_Player->SetPlayerHPUI(player);
					}
				}
			}
			BleedingProcess(player->battlePlayerState);
		}
		else if (auto* enemy = Cast<ABaseEnemy>(this))
		{
			enemy->enemybattleState->hp -= 5;
			if (UBattleUnitStateUI* ui = Cast<UBattleUnitStateUI>(enemy->battleUnitStateComp->GetWidget()))
			{
				ui->UpdateHP(enemy->enemybattleState->hp);
			}
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
	UE_LOG(LogTemp, Warning, TEXT("BuildPath: pathArray.Num = %d, moveRange = %d"), pathArray.Num(), moveRange);
	UE_LOG(LogTemp, Warning, TEXT("Final bIsMoving = %s"), bIsMoving ? TEXT("true") : TEXT("false"));
	if (openArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenArray is empty"));
		return;
	}

	int32 safetyCounter = 0;
	constexpr int32 maxSafetyCount = 1000;

	while (openArray.Num() > 0 && safetyCounter++ < maxSafetyCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("BuildPath: pathArray.Num = %d, moveRange = %d"), pathArray.Num(), moveRange);
		UE_LOG(LogTemp, Warning, TEXT("Final bIsMoving = %s"), bIsMoving ? TEXT("true") : TEXT("false"));
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
	AGridTileManager* tileManger = Cast<AGridTileManager>(UGameplayStatics::GetActorOfClass(GetWorld(), TileManagerFactory));

	if (AGridTile* tile = tileManger->map.FindRef(nextCoord))
	{
		float previousCost = tile->tCostValue;
		tile->SetCost(currentTile, goalTile);

		// if (!tile->parentTile || tile->tCostValue < previousCost)
		// {
		// 	tile->parentTile = currentTile;
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
	
	// goalTile 또는 goalTile->parentTile 자체가 nullptr일 수 있다.
	if (!goalTile || !goalTile->parentTile)
	{
		UE_LOG(LogTemp, Error, TEXT("BuildPath aborted: goalTile or parent is null"));
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
		UE_LOG(LogTemp, Warning, TEXT("Path too long! moveRange = %d, path length = %d"), moveRange, pathArray.Num());
		pathArray.SetNum(moveRange); // 최대 이동 가능 거리만큼 잘라 이동
	}
	
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

void ABaseBattlePawn::OnMoveEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("Move Complete"));
	OnTurnEnd();
}

void ABaseBattlePawn::InitValues()
{
	AGridTileManager* tileManager = Cast<AGridTileManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), TileManagerFactory));
	if (!tileManager)
	{
		return;
	}

	for (auto& pair : tileManager->map)
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
	// 내가 컨트롤하고 있는 카메라를 가져오자
	AActor* cam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	// 카메라의 앞방향(반대), 윗방향을 이용해서 Rotator를 구하자.
	FRotator rot = UKismetMathLibrary::MakeRotFromXZ(-cam->GetActorForwardVector(), cam->GetActorUpVector());
	// battleUnitStateComp를 구한 Rotator 값으로 설정.
	battleUnitStateComp->SetWorldRotation(rot);
}

void ABaseBattlePawn::OnMouseHover()
{
	FVector start, end, dir;
	FHitResult hitInfo;
	FCollisionQueryParams params;

	GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(start, dir);
	end = start + dir * 10000;

	bool bHitUnit = false;

	if (GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Visibility, params))
	{
		if (ABaseBattlePawn* hoveredUnit = Cast<ABaseBattlePawn>(hitInfo.GetActor()))
		{
			// 유닛에 마우스를 올리고 있을 때
			if (UBattleUnitStateUI* ui = Cast<UBattleUnitStateUI>(hoveredUnit->battleUnitStateComp->GetWidget()))
			{
				hoveredUnit->battleUnitStateComp->SetDrawSize(FVector2D(150, 130));
				ui->ShowHoverUI();
				lastHoveredPawn = hoveredUnit;
				bHitUnit = true;
			}
		}
	}

	// 이전에 호버 중이었던 유닛이 있었는데 지금은 안 겹친 경우 → Base UI로 전환
	if (!bHitUnit && lastHoveredPawn)
	{
		if (UBattleUnitStateUI* ui = Cast<UBattleUnitStateUI>(lastHoveredPawn->battleUnitStateComp->GetWidget()))
		{
			lastHoveredPawn->battleUnitStateComp->SetDrawSize(FVector2D(50, 10));
			ui->ShowBaseUI();
		}
		lastHoveredPawn = nullptr;
	}
}
