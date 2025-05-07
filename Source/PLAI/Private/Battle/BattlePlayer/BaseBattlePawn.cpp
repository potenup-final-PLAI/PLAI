// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/BattlePlayer/BaseBattlePawn.h"

#include "BasePlayerState.h"
#include "EnhancedInputSubsystems.h"
#include "GridTile.h"
#include "GridTileManager.h"
#include "Battle/Http/BattleHttpActor.h"
#include "Battle/TurnSystem/PhaseManager.h"
#include "Battle/TurnSystem/TurnManager.h"
#include "Battle/UI/BattleHUD.h"
#include "Battle/UI/MainBattleUI.h"
#include "Enemy/BaseEnemy.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BattlePlayer.h"
#include "Player/BattlePlayerState.h"

// Sets default values
ABaseBattlePawn::ABaseBattlePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseBattlePawn::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;

	turnManager = Cast<ATurnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), turnManagerFactory));

	if (ABasePlayerState* ps = Cast<ABasePlayerState>(GetPlayerState()))
	{
		ps->SetAP();
	}

}

// Called every frame
void ABaseBattlePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsMoving && pathArray.IsValidIndex(currentPathIndex))
	{
		FVector targetLoc = pathArray[currentPathIndex]->GetActorLocation() + FVector(0, 0, 10);
		FVector currentLoc = GetActorLocation();
		FVector nextLoc = FMath::VInterpConstantTo(currentLoc, targetLoc, DeltaTime, 500.f);
		SetActorLocation(nextLoc);

		if (FVector::DistSquared(currentLoc, targetLoc) < FMath::Square(5.f))
		{
			++currentPathIndex;
			if (!pathArray.IsValidIndex(currentPathIndex))
			{
				bIsMoving = false;
				currentTile = pathArray.Last();
				pathArray.Empty();

				if (auto* enemy = Cast<ABaseEnemy>(this))
				{
					// Player 찾아서 공격  
					enemy->FindAndAttackPlayer();
				}
				// else
				// {
				// 	// 이동 끝났으면 턴 종료 처리
				// 	OnTurnEnd();
				// }
			}
		}
	}
}

void ABaseBattlePawn::OnTurnStart()
{
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
					
					FString s = Cast<ABattlePlayer>(turnManager->curUnit) ? TEXT("Player") : TEXT("Enemy");
					hud->mainUI->WBP_CycleAndTurn->SetTurnText(s);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TurnManager is nullptr"));
	}
	// 상태이상이 있다면 대미지 및 버프, 디버프 처리하기
	ApplyStatusEffect();

	// Player라면
	if (ABattlePlayer* player = Cast<ABattlePlayer>(this))
	{
		// 턴 시작 시 AP 증가
		if (ABattlePlayerState* ps = Cast<ABattlePlayerState>(GetPlayerState()))
		{
			ps->curAP += ps->GetAP(ps->curAP);
			UE_LOG(LogTemp, Warning, TEXT("Current AP: %d"), ps->curAP);
		}
	}
	// Enemy라면
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(this))
	{
		// 턴 시작 시 AP 증가
		enemybattleState->curAP += enemybattleState->GetAP(enemybattleState->curAP);
		UE_LOG(LogTemp, Warning, TEXT("Current AP: %d"), enemybattleState->curAP);
		
		UE_LOG(LogTemp, Warning, TEXT("BaseBattlePawn::OnTurnStart"));
		ABaseBattlePawn* CapturedUnit = this;
		
		FTimerHandle battleAPIHandle;
		GetWorld()->GetTimerManager().SetTimer(battleAPIHandle, FTimerDelegate::CreateLambda([this, CapturedUnit]()
		{
			UE_LOG(LogTemp, Warning, TEXT("BaseBattlePawn::In Lambda"));
			if (turnManager && turnManager->phaseManager)
			{
				UE_LOG(LogTemp, Warning, TEXT("BaseBattlePawn::turnManager, phaseManager is Set"));
				turnManager->phaseManager->TrySendbattleState(CapturedUnit);
			}
		}), 4.0f, false);
	}
	UE_LOG(LogTemp, Warning, TEXT("%s Turn Start"), *GetName());
}
// 자동 탐색 전 가까운 적을 찾는 로직
// AGridTileManager* tileManger = Cast<AGridTileManager>(UGameplayStatics::GetActorOfClass(GetWorld(), TileManagerFactory));
//
// TArray<AActor*> playerActors;
// UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABattlePlayer::StaticClass(), playerActors);
//
// TArray<ABattlePlayer*> players;
// for (AActor* actor : playerActors)
// {
// 	if (ABattlePlayer* p = Cast<ABattlePlayer>(actor))
// 	{
// 		players.Add(p);
// 	}
// }
//
// if (ABattlePlayer* target = enemy->FindClosestPlayer(players))
// {
// 	enemy->MoveToPlayer(target, tileManger);
// }
		
// FTimerHandle timerHandle;
// GetWorld()->GetTimerManager().SetTimer(timerHandle, FTimerDelegate::CreateLambda([this]()
// {
// 	OnTurnEnd();
// }), 5.0f, false);
void ABaseBattlePawn::OnTurnEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("%s Turn End"), *GetName());
	// 입력 막고 FSM 종료
	
	// Casting을 통해 현재 유닛이 player 또는 enemy라면 그쪽 함수 실행
	if (auto* phaseManager = Cast<AUPhaseManager>(GetWorld()->GetGameState()))
	{
		// 턴 종료 상태라면 return
		if (phaseManager->turnManager->curTurnState == ETurnState::TurnEnd) return;
		
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

	GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(start, dir);
	end = start + dir * 10000;

	if (GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Visibility, params))
	{
		// 상대와 거리 측정
		float dist = FVector::Dist(GetActorLocation(), hitInfo.GetActor()->GetActorLocation());
		
		// 자신의 moveRange * 100 보다 작거나 같을 때
		if (dist > (this->battlePlayerState->playerStatus.move_Range * 100))
		{
			UE_LOG(LogTemp, Warning, TEXT("Test Click : Dist too be far"));
			return;
		}

		EActionMode curSkillState = currentActionMode;
		UE_LOG(LogTemp, Warning, TEXT("Test Click : Current Action Mode %s"), *UEnum::GetValueAsString(curSkillState));
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
	// 이동을 위한 타일이라면
	if (AGridTile* testTile = Cast<AGridTile>(hitInfo.GetActor()))
	{
		FVector targetLoc = testTile->GetActorLocation();
		SetActorLocation(targetLoc);
		// goalTile 쪽으로 이동
		targetTile = testTile;
		currentTile = testTile;
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
	// 공격을 위한 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		ApplyAttack(enemy, EActionMode::BaseAttack);
	}
	// 공격을 위한 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		ApplyAttack(player, EActionMode::BaseAttack);
	}
}

void ABaseBattlePawn::PlayerParalysis(FHitResult& hitInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerParalysis In"));
	// 공격을 위한 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerParalysis In Enemy"));
		ApplyAttack(enemy, EActionMode::Paralysis);
	}
	// 공격을 위한 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerParalysis In Player"));
		ApplyAttack(player, EActionMode::Paralysis);
	}
}

void ABaseBattlePawn::PlayerPoison(FHitResult& hitInfo)
{
	// 공격을 위한 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		ApplyAttack(enemy, EActionMode::Poison);
	}
	// 공격을 위한 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		ApplyAttack(player, EActionMode::Poison);
	}
}

void ABaseBattlePawn::PlayerVulnerable(FHitResult& hitInfo)
{
	// 공격을 위한 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		ApplyAttack(enemy, EActionMode::Vulnerable);
	}
	// 공격을 위한 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		ApplyAttack(player, EActionMode::Vulnerable);
	}
}

void ABaseBattlePawn::PlayerWeaking(FHitResult& hitInfo)
{
	// 공격을 위한 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		ApplyAttack(enemy, EActionMode::Weakening);
	}
	// 공격을 위한 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		ApplyAttack(player, EActionMode::Weakening);
	}
}

void ABaseBattlePawn::PlayerFatal(FHitResult& hitInfo)
{
	// 공격을 위한 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		ApplyAttack(enemy, EActionMode::Fatal);
	}
	// 공격을 위한 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		ApplyAttack(player, EActionMode::Fatal);
	}
}

void ABaseBattlePawn::PlayerRupture(FHitResult& hitInfo)
{
	// 공격을 위한 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		ApplyAttack(enemy, EActionMode::Rupture);
	}
	// 공격을 위한 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		ApplyAttack(player, EActionMode::Rupture);
	}
}

void ABaseBattlePawn::PlayerRoar(FHitResult& hitInfo)
{
	// 공격을 위한 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		ApplyAttack(enemy, EActionMode::Roar);
	}
	// 공격을 위한 player라면
	else if (ABattlePlayer* player = Cast<ABattlePlayer>(hitInfo.GetActor()))
	{
		ApplyAttack(player, EActionMode::Roar);
	}
}

void ABaseBattlePawn::PlayerBattleCry(FHitResult& hitInfo)
{
	// 공격을 위한 enemy라면
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(hitInfo.GetActor()))
	{
		ApplyAttack(enemy, EActionMode::BattleCry);
	}
	// 공격을 위한 player라면
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
		
		// 전송용 구조체에 값 세팅
		enemybattleState->enemyStatus.hp = enemybattleState->hp;
		enemybattleState->enemyStatus.attack = enemybattleState->attack;
		enemybattleState->enemyStatus.defense = enemybattleState->defense;
		enemybattleState->enemyStatus.resistance = enemybattleState->resistance;
		enemybattleState->enemyStatus.move_Range = enemybattleState->move_Range;
		enemybattleState->enemyStatus.critical_Rate = enemybattleState->critical_Rate;
		enemybattleState->enemyStatus.critical_Damage = enemybattleState->critical_Damage;
		enemybattleState->enemyStatus.speed = enemybattleState->speed;
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
		UE_LOG(LogTemp, Warning, TEXT("Damage : %d, playerHP : %d"), damage, player->battlePlayerState->playerStatus.hp);

		// hp가 0보다 작으면 사망
		if (player->battlePlayerState->playerStatus.hp <= 0)
		{
			player->battlePlayerState->playerLifeState = ELifeState::Dead;
			UE_LOG(LogTemp, Warning, TEXT("Enemy Dead %s"), *UEnum::GetValueAsString(player->battlePlayerState->playerLifeState));	
		}
	}
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(unit))
	{
		// 피를 깎는다.
		enemy->enemybattleState->enemyStatus.hp = FMath::Max(0, enemy->enemybattleState->enemyStatus.hp - damage);
		UE_LOG(LogTemp, Warning, TEXT("Damage : %d, enemyHP : %d"), damage, enemy->enemybattleState->enemyStatus.hp);

		// hp가 0보다 작으면 사망
		if (enemy->enemybattleState->hp <= 0)
		{
			enemy->enemybattleState->enemyLifeState = ELifeState::Dead;
			UE_LOG(LogTemp, Warning, TEXT("Enemy Dead %s"), *UEnum::GetValueAsString(enemy->enemybattleState->enemyLifeState)); 
		}
	}

	// 대미지를 줬다면 현재 공격 상태를 None으로 초기화
	currentActionMode = EActionMode::None;
}

void ABaseBattlePawn::ApplyAttack(ABaseBattlePawn* targetUnit, EActionMode attackType)
{
	UE_LOG(LogTemp, Warning, TEXT("[ApplyAttack] Called by %s with attackType %d"), *GetName(), (int32)attackType);
	
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
			int32 personality = 2;
			int32 status_effect = 4;
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
			if (attackType == EActionMode::Roar || attackType == EActionMode::BattleCry) return;
			
			// 반올림 처리
			int32 chance = FMath::RoundToInt(critical * 100.0f);
			int32 roll = FMath::RandRange(1, 100);

			
			if (bool bIsCrit = roll <= chance)
			{
				int32 critical_Damage = enemybattleState->enemyStatus.critical_Damage;
				damage = ((atk + weapon) * skillMultiplier * (1 + (personality + status_effect))) * critical_Damage;
				UE_LOG(LogTemp, Warning, TEXT("Critical Damage : %d"), damage);
			}
			else
			{
				damage = (atk + weapon) * skillMultiplier * (1 + (personality + status_effect));
				UE_LOG(LogTemp, Warning, TEXT("Damage : %d"), damage);
			}
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
			int32 personality = 2;
			int32 status_effect = 4;
			int32 damage = 0;

			
			// 스킬 계수 및 추가 효과
			float skillMultiplier = 1.0f;
			
			switch (attackType)
			{
				case EActionMode::Paralysis:
					// 현재 Ap가 cost보다 크다면 실행
					if (!baseAttackPlayer->battlePlayerState->CanConsumeAP(1))
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
					if (!baseAttackPlayer->battlePlayerState->CanConsumeAP(1))
					{
						UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
						return;
					}
					enemy->AddStatusEffect(EStatusEffect::Poison, 3);
					break;
				case EActionMode::Vulnerable:
					// 현재 Ap가 cost보다 크다면 실행
					if (!baseAttackPlayer->battlePlayerState->CanConsumeAP(1))
					{
						UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
						return;
					}
					enemy->AddStatusEffect(EStatusEffect::Vulnerable, 2);
					break;
				case EActionMode::Weakening:
					// 현재 Ap가 cost보다 크다면 실행
					if (!baseAttackPlayer->battlePlayerState->CanConsumeAP(1))
					{
						UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
						return;
					}
					enemy->AddStatusEffect(EStatusEffect::Weakening, 2);
					break;
				case EActionMode::Fatal:
					// 현재 Ap가 cost보다 크다면 실행
					if (!baseAttackPlayer->battlePlayerState->CanConsumeAP(2))
					{
						UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
						return;
					}
					skillMultiplier = 1.5f;
					enemy->AddStatusEffect(EStatusEffect::Bleeding, 2);
					break;
				case EActionMode::Rupture:
					// 현재 Ap가 cost보다 크다면 실행
					if (!baseAttackPlayer->battlePlayerState->CanConsumeAP(2))
					{
						UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
						return;
					}
					skillMultiplier = 1.5f;
					enemy->AddStatusEffect(EStatusEffect::Vulnerable, 1);
					break;
				case EActionMode::Roar:
					// 현재 Ap가 cost보다 크다면 실행
					if (!baseAttackPlayer->battlePlayerState->CanConsumeAP(1))
					{
						UE_LOG(LogTemp, Warning, TEXT("Can't Use Skill"));
						return;
					}
					enemy->AddStatusEffect(EStatusEffect::Weakening, 2);
					break;
				case EActionMode::BattleCry:
					// 현재 Ap가 cost보다 크다면 실행
					if (!baseAttackPlayer->battlePlayerState->CanConsumeAP(2))
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
			if (attackType == EActionMode::Roar || attackType == EActionMode::BattleCry) return;
			
			// 반올림 처리
			int32 chance = FMath::RoundToInt(critical * 100.0f);
			int32 roll = FMath::RandRange(1, 100);

			
			if (bool bIsCrit = roll <= chance)
			{
				int32 critical_Damage = battlePlayerState->playerStatus.critical_Damage;
				damage = ((atk + weapon) * skillMultiplier * (1 + (personality + status_effect))) * critical_Damage;
				UE_LOG(LogTemp, Warning, TEXT("Critical Damage : %d"), damage);
			}
			else
			{
				damage = (atk + weapon) * skillMultiplier * (1 + (personality + status_effect));
				UE_LOG(LogTemp, Warning, TEXT("Damage : %d"), damage);
			}
			GetDamage(enemy, damage);
		}
	}
}



void ABaseBattlePawn::AddStatusEffect(EStatusEffect newEffect, int32 duration)
{
	if (activeStatusEffects.Contains(newEffect))
	{
		// 이미 있으면 지속시간 갱신 or 덧붙이기
		activeStatusEffects[newEffect] = FMath::Max(activeStatusEffects[newEffect], duration);
		// UEnum로그를 찍기 위해 enum 메타데이터를 얻어서 Log찍는 과정 
		UEnum* enumPtr = StaticEnum<EStatusEffect>();
		if (enumPtr)
		{
			UE_LOG(LogTemp, Warning, TEXT("In AddStatusEffect : %s, Duration : %d"),
				*enumPtr->GetNameStringByValue(static_cast<int64>(newEffect)),
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
		}
		else if (auto* enemy = Cast<ABaseEnemy>(this))
		{
			enemy->enemybattleState->hp -= 5;
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
			BleedingProcess(player->battlePlayerState);
		}
		else if (auto* enemy = Cast<ABaseEnemy>(this))
		{
			enemy->enemybattleState->hp -= 5;
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

void ABaseBattlePawn::MouseClick(const FInputActionValue& value)
{
	if (!bIsMoveMode) return;
	
	FHitResult hitInfo;
	FVector start, end, dir;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	// 마우스 클릭 위치 얻기
	GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(start, dir);
	end = start + dir * 10000;

	if (GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Visibility, params))
	{
		AGridTile* clickTile = Cast<AGridTile>(hitInfo.GetActor());
		if (clickTile)
		{
			goalTile = clickTile;

			// 시작 타일 다시 설정
			FHitResult startHit;
			FVector pawnStart = GetActorLocation();
			FVector pawnEnd = pawnStart + FVector::DownVector * 1000;

			if (GetWorld()->LineTraceSingleByChannel(startHit, pawnStart, pawnEnd, ECC_Visibility, params))
			{
				// 시작 타일 직접 설정
				startTile = Cast<AGridTile>(startHit.GetActor());
				if (IsValid(startTile))
				{
					// 시작 비용 초기화
					startTile->sCostValue = 0;
					openArray.Add(startTile);
					// 길찾기 시작
					PathFind();
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("StartTile is Invalid Cannot start pathFinding"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Fail to LineTrace downwards to find clickTile"));
			}

			bIsMoveMode = false; // 클릭했으니까 모드 종료
		}
		
	}
}

void ABaseBattlePawn::PathFind()
{
	if (openArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenArray is empty"));
		return;
	}

	int32 safetyCounter = 0;
	const int32 maxSafetyCount = 1000;
	
	while (openArray.Num() > 0 && safetyCounter++ < maxSafetyCount)
	{
		if (safetyCounter > maxSafetyCount)
		{
			UE_LOG(LogTemp, Warning, TEXT("Path Safe Break"));
			break;
		}
		currentTile = openArray[0];
		openArray.RemoveAt(0);

		if (!IsValid(currentTile))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s currentTile PathFind Error"), *GetName());
			return;
		}
		if (!IsValid(goalTile))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s goalTile PathFind Error"), *GetName());
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

		if (!tile->parentTile || tile->tCostValue < previousCost)
		{
			tile->parentTile = currentTile;
			
			int32 i = 0;
			for (; i < openArray.Num(); ++i)
			{
				if (openArray[i]->tCostValue >= tile->tCostValue) break;
			}
			openArray.Insert(tile, i);
		}
		// if (!openArray.Contains(tile) && !closedArray.Contains(tile))
		// {
		// 	tile->SetCost(currentTile, goalTile);
		// 	int32 i = 0;
		// 	for (i = 0; i < openArray.Num(); ++i)
		// 	{
		// 		if (openArray[i]->tCostValue >= tile->tCostValue) break;
		// 	}
		// 	openArray.Insert(tile, i);
		// }
	}
}
void ABaseBattlePawn::BuildPath()
{
	TSet<AGridTile*> visitePathTiles;
	
	// 찾은 길 표시
	AGridTile* temp = goalTile;
	while (temp && temp->parentTile)
	{
		if (visitePathTiles.Contains(temp))
		{
			UE_LOG(LogTemp, Warning, TEXT(" Infinite loop detected in BuildPath"));
			break;
		}
		visitePathTiles.Add(temp);
		pathArray.Insert(temp, 0); // 역방향으로 삽입
		temp = temp->parentTile;
	}

	if (auto* player = Cast<ABattlePlayer>(turnManager->curUnit))
	{
		player->moveRange = player->battlePlayerState->playerStatus.move_Range;
	}
	else if (auto* enemy = Cast<ABaseEnemy>(turnManager->curUnit))
	{
		enemy->moveRange = enemy->enemybattleState->move_Range;
	}
	
	// 경로 저장 완료했으면 이동 시작
	if (pathArray.Num() > moveRange)
	{
		UE_LOG(LogTemp, Warning, TEXT("Path too long! Max Move Range = %d, Path Length = %d"), moveRange, pathArray.Num());
        
		// 경로 초기화하고 이동 금지
		pathArray.Empty();
		bIsMoving = false;

		return;
	}
	
	if (pathArray.Num() > 0)
	{
		bIsMoving = true;
		currentPathIndex = 0;
	}
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
				if (openArray[i]->tCostValue >= tile->tCostValue)
				{
					break;
				}
			}
			openArray.Insert(tile, i);
		}
	}
}

void ABaseBattlePawn::UnitMove()
{
	// 유닛이 찾은 길을 따라 움직이기
	if (pathArray.IsValidIndex(currentPathIndex))
	{
		FVector targetLoc = pathArray[currentPathIndex]->GetActorLocation();
		FVector currentLoc = GetActorLocation();

		FVector newLoc = FMath::VInterpConstantTo(currentLoc, targetLoc, GetWorld()->GetDeltaSeconds(), moveSpeed);

		SetActorLocation(newLoc);
		
		// 목표 지점에 거의 도착하면 다음 목표로 이동
		if (FVector::DistSquared(newLoc, targetLoc) < FMath::Square(5.0f))
		{
			SetActorLocation(targetLoc);
			++currentPathIndex;

			// 인덱스가 array보다 크거나 같다면
			if (currentPathIndex >= pathArray.Num())
			{
				// 이동 종료
				bIsMoving = false;
				// AI 액션 처리를 위한 함수
				OnMoveEnd();
			}
		}
	}
	else
	{
		bIsMoving = false;
	}
}

void ABaseBattlePawn::OnMoveEnd()
{
	// AI에 턴을 종료하거나 다음 액션 처리 등을 여기서 처리
	UE_LOG(LogTemp, Warning, TEXT("Move Complete"));
}

void ABaseBattlePawn::InitValues()
{
	AGridTileManager* tileManager = Cast<AGridTileManager>(UGameplayStatics::GetActorOfClass(GetWorld(), TileManagerFactory));
	if (!tileManager) return;
	
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
		if (IsValid(tile)) tile->parentTile = nullptr;
	}
    
	for (AGridTile* tile : openArray)
	{
		if (IsValid(tile)) tile->parentTile = nullptr;
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

