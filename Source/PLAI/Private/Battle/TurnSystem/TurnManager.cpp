// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/TurnSystem/TurnManager.h"

#include "BaseBattlePawn.h"
#include "GridTile.h"
#include "GridTileManager.h"
#include "Battle/TurnSystem/BattlePlayerController.h"
#include "Battle/TurnSystem/PhaseManager.h"
#include "Battle/UI/BattleHUD.h"
#include "Battle/UI/MainBattleUI.h"
#include "Battle/Util/DebugHeader.h"
#include "Enemy/BaseEnemy.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
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
		player->ClientRPC_AddAP(player);
		player->MultiCastRPC_InitAPUI();
	
		UE_LOG(LogTemp, Warning, TEXT("%s -> curAP : %d"), *player->GetName(),player->curAP);
	}
	// Enemy라면
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(curUnit))
	{
		UE_LOG(LogTemp, Warning, TEXT("BaseBattlePawn::OnTurnStart"));

		// 턴 시작 시 AP 증가
		enemy->GetAP();
		UE_LOG(LogTemp, Warning, TEXT("enemy Current AP: %d"),enemy->curAP);

		// 현재 위치에 타일을 가져와서
		if (auto* tile = phaseManager->gridTileManager->FindCurrentTile(enemy->GetActorLocation()))
		{
			// 현재 타일 설정
			NET_PRINTLOG(TEXT(""));
			enemy->currentTile = tile;
			enemy->MulticastRPC_EnemyTile(tile);
		}
		
		// 이동 범위 보이게
		enemy->Multicast_SeeMoveRange();
		
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

void ATurnManager::Multicast_AddOrderUnit_Implementation()
{
	// 턴 시작할 때 TurnOrder UI 초기화해서 다시 세팅
	if (phaseManager->hud)
	{
		phaseManager->hud->mainUI->AddOrder(phaseManager->turnOrderArray);
	}
	else if (phaseManager->turnOrderArray.Num() > 0)
	{
		NET_PRINTLOG(TEXT("Turn order array is empty or hud is Null"));
	}
	else
	{
		NET_PRINTLOG(TEXT("phaseManager->hud"));
	}
	
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
	
	// 턴이 종료 되면 turnOrderArray에 0번을 빼서 orderUnit에 담아놓고 RmoveAt 후 다시 Add로 추가
	// 순서 바꾸고 UI 갱신 호출
	Multicast_RemoveOrderUnit();
	Multicast_AddOrderUnit();
	
	
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

void ATurnManager::Multicast_RemoveOrderUnit_Implementation()
{
	// 턴이 종료 되면 turnOrderArray에 0번을 빼서 orderUnit에 담아놓고 RmoveAt 후 다시 Add로 추가
	if (phaseManager && phaseManager->turnOrderArray.Num() > 0)
	{
		auto* orderUnit = phaseManager->turnOrderArray[0];
		phaseManager->turnOrderArray.RemoveAt(0);
		phaseManager->turnOrderArray.Add(orderUnit);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ATurnManager::OnTurnEnd : phaseManager && phaseManager->turnOrderArray.Num() > 0"));
	}
}
