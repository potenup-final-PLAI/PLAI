// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStateOpen.h"

#include "Kismet/GameplayStatics.h"
#include "PLAI/Item/Turn/TurnMoster/TurnMonsterFsm.h"

AGameStateOpen::AGameStateOpen()
{
}

void AGameStateOpen::BeginPlay()
{
	Super::BeginPlay();
}

void AGameStateOpen::NextPlayerTurn(ATurnPlayer* TurnPlayer)
{
	if (TurnPlayers.Num() <= 0) { return; }

	TurnPlayers.Remove(TurnPlayer);

	if (TurnPlayers.Num() <= 0) { return; }
	
	for (int i = 0; i < TurnPlayers.Num(); i++)
	{
		TurnPlayers[i]->TurnIndex = i;
		UE_LOG(LogTemp,Warning,TEXT("Gamestate Player Index:[%d] bTurn?[%d] "),TurnPlayers[i]->TurnIndex,TurnPlayers[i]->bTurn);
	}
	if (TurnPlayers[0])
	{ TurnPlayers[0]->bTurn = true; }
}

void AGameStateOpen::FindPlayerTurn()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ATurnPlayer::StaticClass(),Actors);
	for(int i = 0; i < Actors.Num(); i++)
	{
		if (ATurnPlayer* TurnPlayer = Cast<ATurnPlayer>(Actors[i]))
		{
			TurnPlayer->TurnIndex = i;
			TurnPlayers.Add(TurnPlayer);
		}
	}
	TurnPlayers[0]->bTurn = true;
}

void AGameStateOpen::FindMonsterTurn()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ATurnMonster::StaticClass(),Actors);
	for(int i = 0; i < Actors.Num(); i++)
	{
		if (ATurnMonster* TurnMonster = Cast<ATurnMonster>(Actors[i]))
		{
			TurnMonster->TurnIndex = i;
			TurnMonsters.Add(TurnMonster);
		}
	}
	TurnMonsters[0]->bTurn = true;
}

void AGameStateOpen::NextMonsterTurn()
{
	if (TurnMonsters.Num() <= 0){ return; }
	
	TurnMonsters.RemoveAt(0);
	
	if (TurnMonsters.Num() <= 0){ return; }
	
	for (int i = 0; i < TurnMonsters.Num(); i++)
	{
		TurnMonsters[i]->TurnIndex = i;
	}
	if (TurnMonsters[0])
	{
		TurnMonsters[0]->bTurn = true;
	}
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
	{
		TimerSecond += GetWorld()->GetDeltaSeconds();
		if (TimerSecond > 2)
		{
			TurnMonsters[0]->TurnMonsterFsm->MoveToPlayer();
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		}
	},0.02f,true);
}

void AGameStateOpen::NextRound()
{
}




// int32 Index = Actors.IndexOfByKey(SomeActor);
