// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnMonsterFsm.h"

#include "TurnMonster.h"
#include "Navigation/PathFollowingComponent.h"
#include "PLAI/Item/GameState/GameStateOpen.h"
#include "PLAI/Item/UI/Turn/UITurnHpBar.h"


// Sets default values for this component's properties
UTurnMonsterFsm::UTurnMonsterFsm()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTurnMonsterFsm::BeginPlay()
{
	Super::BeginPlay();

	GameState = Cast<AGameStateOpen>(GetWorld()->GetGameState());
	TurnMonster = Cast<ATurnMonster>(GetOwner());
}


// Called every frame
void UTurnMonsterFsm::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (TurnMonsterState)
	{
		case ETurnMonsterState::Idle:
		IdleState();
		break;

	    case ETurnMonsterState::Move:
		MoveState();
		break;

	    case ETurnMonsterState::Attack:
		AttackState();
		break;
	}
}

void UTurnMonsterFsm::IdleState()
{
}

void UTurnMonsterFsm::MoveState()
{
}

void UTurnMonsterFsm::AttackState()
{
}

void UTurnMonsterFsm::MoveToPlayer()
{
	if (GameState->TurnPlayers.Num() <= 0){UE_LOG(LogTemp,Warning,TEXT("TurnMonFsm 플레이어 없음"))return;}
	
	int32 rand = FMath::RandRange(0,GameState->TurnPlayers.Num()-1);

	TurnPlayer = GameState->TurnPlayers[rand];
	TurnMonster->AiController->ReceiveMoveCompleted.AddUniqueDynamic(this,&UTurnMonsterFsm::AttackToPlayer);
	TurnMonster->AiController->MoveToLocation(GameState->TurnPlayers[rand]->GetActorLocation(),200,
		true,true,true);

	// GameState->NextMonsterTurn();
}

void UTurnMonsterFsm::AttackToPlayer(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result == EPathFollowingResult::Type::Success && TurnPlayer != nullptr)
	{
		TurnPlayer->TurnPlayerStruct.CurrentHp -= TurnMonster->TurnMonsterStruct.Attack;
		TurnPlayer->UITurnHpBar->SetHpBar(TurnPlayer->TurnPlayerStruct);
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
	{
		GameState->NextMonsterTurn();
	},2.0f,false);
}
