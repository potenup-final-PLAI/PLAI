// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/BattlePlayer/BaseBattlePawn.h"

#include "Battle/TurnSystem/PhaseManager.h"
#include "Battle/TurnSystem/TurnManager.h"
#include "Developer/AITestSuite/Public/AITestsCommon.h"
#include "Enemy/BaseEnemy.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BattlePlayer.h"

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
}

// Called every frame
void ABaseBattlePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABaseBattlePawn::SetupPlayerInputComponent(
	UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseBattlePawn::OnTurnStart()
{
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(this))
	{
		FTimerHandle enemyNextTurnimerHandle;
		GetWorld()->GetTimerManager().SetTimer(enemyNextTurnimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			if (auto* phaseManager = Cast<AUPhaseManager>(GetWorld()->GetGameState()))
			{
				phaseManager->EndEnemyPhase();
			}
		}), 1.5f, false);
	}
	UE_LOG(LogTemp, Warning, TEXT("%s Turn Start"), *GetName());
}

void ABaseBattlePawn::OnTurnEnd(ABaseBattlePawn* unit)
{
	UE_LOG(LogTemp, Warning, TEXT("%s Turn End"), *GetName());
	// 입력 막고 FSM 종료

	// 포제스 풀기
	// if (auto* pc = GetWorld()->GetFirstPlayerController())
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("%s UnPossess"), *unit->GetActorNameOrLabel());
	// 	pc->UnPossess();
	// }
	
	// 다음 턴으로 넘기기
	// if (auto* pm = GetWorld()->GetGameState<AUPhaseManager>())
	// {
	// 	pm->RequestNextTurn(this);		
	// }
	// Casting을 통해 현재 유닛이 player 또는 enemy라면 그쪽 함수 실행
	auto* phaseManager = Cast<AUPhaseManager>(GetWorld()->GetGameState());
	if (ABattlePlayer* player = Cast<ABattlePlayer>(unit))
	{
		// PlayerPhaseEnd

		phaseManager->EndPlayerPhase();
	}
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(unit))
	{
		// EnemyPhaseEnd
		phaseManager->EndEnemyPhase();
	}
}
