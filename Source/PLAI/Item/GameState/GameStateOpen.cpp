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

void AGameStateOpen::FindPlayerTurn()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ATurnPlayer::StaticClass(),Actors);
	for(AActor* Actor : Actors)
	{
		if (ATurnPlayer* TurnPlayer = Cast<ATurnPlayer>(Actor))
		{ TurnPlayers.Add(TurnPlayer); }
	}
}

void AGameStateOpen::PlayerTurn()
{
	
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
