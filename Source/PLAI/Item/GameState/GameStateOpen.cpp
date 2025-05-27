// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStateOpen.h"

#include "Kismet/GameplayStatics.h"

AGameStateOpen::AGameStateOpen()
{
}

void AGameStateOpen::BeginPlay()
{
	Super::BeginPlay();
}

void AGameStateOpen::NextPlayerTurn(ATurnPlayer* TurnPlayer)
{
	TurnPlayers.Remove(TurnPlayer);
	for (int i = 0; i < TurnPlayers.Num(); i++)
	{
		if (i == 0){ TurnPlayer[i].bTurn = true; }
		
		TurnPlayers[i]->TurnIndex = i;
	}
}

void AGameStateOpen::FindPlayerTurn()
{
	// int32 Index = Actors.IndexOfByKey(SomeActor);
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
}

void AGameStateOpen::FindMonsterTurn()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ATurnMonster::StaticClass(),Actors);
	for(AActor* Actor : Actors)
	{
		if (ATurnMonster* TurnMonster = Cast<ATurnMonster>(Actor))
		{ TurnMonsters.Add(TurnMonster); }
	}
}

void AGameStateOpen::MonsterTurn()
{
	
}
