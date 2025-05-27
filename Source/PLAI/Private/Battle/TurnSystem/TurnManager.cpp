// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/TurnSystem/TurnManager.h"

#include "BaseBattlePawn.h"
#include "Battle/TurnSystem/BattlePlayerController.h"
#include "Battle/TurnSystem/PhaseManager.h"
#include "Battle/UI/BattleHUD.h"
#include "Battle/UI/MainBattleUI.h"
#include "Battle/Util/DebugHeader.h"
#include "Enemy/BaseEnemy.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/BattlePlayer.h"

DEFINE_LOG_CATEGORY(TPS);

// Sets default values
ATurnManager::ATurnManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ATurnManager::BeginPlay()
{
	Super::BeginPlay();
	phaseManager = Cast<AUPhaseManager>(GetWorld()->GetGameState());
}

void ATurnManager::GetLifetimeReplicatedProps(
	TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATurnManager, curTurnState);
	DOREPLIFETIME(ATurnManager, curUnit);
	DOREPLIFETIME(ATurnManager, bTurnEnded);
}

// Called every frame
void ATurnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		switch (curTurnState)
		{
		case ETurnState::None:
			break;
		case ETurnState::PreparePlayerTurn:
			OnTurnStart();
			curTurnState = ETurnState::PlayerTurn;
			break;
		case ETurnState::PlayerTurn:
			break;
		case ETurnState::PrepareEnemyTurn:
			OnTurnStart();
			curTurnState = ETurnState::EnemyTurn;
			break;
		case ETurnState::EnemyTurn:
			break;
		case ETurnState::TurnEnd:
			break;
		}
	}
}

void ATurnManager::SetTurnState(ETurnState newTurnState)
{
	curTurnState = newTurnState;
}

void ATurnManager::MutliCastRPC_UpdateWhoTurn_Implementation(const FString& turnUnitName)
{
	// 턴 유닛 이름 UI에 세팅
	if (phaseManager->pc && phaseManager->hud && phaseManager->hud->mainUI && phaseManager->hud->mainUI->WBP_CycleAndTurn)
	{
		phaseManager->hud->mainUI->WBP_CycleAndTurn->SetTurnText(turnUnitName);
	}
}

void ATurnManager::StartTurn()
{
	if (!HasAuthority())
	{
		return;
	}

	SetTurnState(ETurnState::None);

	MutliCastRPC_UpdateWhoTurn(curUnit->MyName);
	MulticastRPC_CameraChange(curUnit);

	if (ABattlePlayer* playerPawn = Cast<ABattlePlayer>(curUnit))
	{
		SetTurnState(ETurnState::PreparePlayerTurn);
	}
	else if (ABaseEnemy* pawn = Cast<ABaseEnemy>(curUnit))
	{
		// FSM 활성화
		FTimerHandle possessHandle;
		GetWorld()->GetTimerManager().ClearTimer(possessHandle);
		// 이후에 값이 변경 및 삭제 될 수 있기 때문에 값 복사로 가져와서 람다 내에서 사용
		GetWorld()->GetTimerManager().SetTimer(possessHandle, FTimerDelegate::CreateLambda([&]()
		{
			SetTurnState(ETurnState::PrepareEnemyTurn);
		}), 1.5f, false);
	}
}

void ATurnManager::MulticastRPC_CameraChange_Implementation(class APawn* target)
{
	if (auto* pc = Cast<ABattlePlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		pc->SetViewTargetMyPawn(target);
	}
}

void ATurnManager::OnTurnStart()
{
	// 호스트가 아니라면 바로 종료  
	//if (!(HasAuthority() && curUnit->IsLocallyControlled()))
	if (!HasAuthority())
		return;

	// 턴 종료 초기화
	bTurnEnded = false;
	
	NET_PRINTLOG(TEXT("OnTurnStart"));
	
	// 기본 공격 초기화
	curUnit->bBaseAttack = true;
	
	// 턴이 시작됐으면 턴 카운트 1 증가
	turnCount++;
	UE_LOG(LogTemp, Warning, TEXT("Turn Count %d"), turnCount);

	curUnit->MultiCastRPC_SetBattlePlayerInfoUI();

	// 상태이상이 있다면 대미지 및 버프, 디버프 처리하기
	curUnit->ApplyStatusEffect();

	// Player라면
	if (ABattlePlayer* player = Cast<ABattlePlayer>(curUnit))
	{
		player->GetAP();
		curUnit->MultiCastRPC_InitAPUI();
		UE_LOG(LogTemp, Warning, TEXT("%s -> curAP : %d"), *player->GetName(),player->curAP);
	}
	// Enemy라면
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(curUnit))
	{
		UE_LOG(LogTemp, Warning, TEXT("BaseBattlePawn::OnTurnStart"));

		// 턴 시작 시 AP 증가
		enemy->GetAP();
		UE_LOG(LogTemp, Warning, TEXT("enemy Current AP: %d"),enemy->curAP);

		// 이동 범위 보이게
		enemy->ServerRPC_SeeMoveRange(enemy->moveRange);
		
		ABaseBattlePawn* CapturedUnit = curUnit;

		FTimerHandle battleAPIHandle;
		GetWorld()->GetTimerManager().SetTimer(battleAPIHandle,FTimerDelegate::CreateLambda([this, CapturedUnit]()
		{
			UE_LOG(LogTemp, Warning,TEXT("BaseBattlePawn::In Lambda"));
			if (phaseManager)
			{
				UE_LOG(LogTemp,Warning,TEXT("BaseBattlePawn::turnManager, phaseManager is Set"));
				phaseManager->TrySendbattleState(CapturedUnit);
			}
		}), 1.0f, false);
	}
	UE_LOG(LogTemp, Warning, TEXT("%s Turn Start"), *GetName());
}

void ATurnManager::OnTurnEnd()
{
	if (!HasAuthority())
		return;
	
	// 턴 종료 상태거나 phaseManager가 없다면 return
	if (!phaseManager && curTurnState == ETurnState::TurnEnd)
	{
		UE_LOG(LogTemp, Warning,TEXT(" OnTurnEnd : !(phaseManager && phaseManager->turnManager->curTurnState == ETurnState::TurnEnd"));
		return;
	}

	// 여러 번 호출 방지
	if (bTurnEnded)
	{
		NET_PRINTLOG(TEXT("ABaseBattlePawn::OnTurnEnd : bTurnEnded true"));
		return;
	}
	NET_PRINTLOG(TEXT("ABaseBattlePawn::OnTurnEnd : bTurnEnded false"));
	
	bTurnEnded = true;
	
	curUnit->MultiCastRPC_ClearGridTile();
	
	UE_LOG(LogTemp, Warning, TEXT("%s Turn End"), *curUnit->MyName);

	if (ABattlePlayer* player = Cast<ABattlePlayer>(curUnit))
	{
		// PlayerPhaseEnd
		NET_PRINTLOG(TEXT("Player Turn End"));
		phaseManager->ServerRPC_EndPlayerPhase_Implementation();
	}
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(curUnit))
	{
		NET_PRINTLOG(TEXT("Enemy Turn End"));
		curUnit->MultiCastRPC_UpdateReason();
		phaseManager->ServerRPC_EndEnemyPhase_Implementation();
	}
}
