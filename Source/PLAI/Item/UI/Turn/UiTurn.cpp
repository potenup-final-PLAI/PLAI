// Fill out your copyright notice in the Description page of Project Settings.


#include "UiTurn.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "PLAI/Item/GameState/GameStateOpen.h"
#include "PLAI/Item/Turn/TurnMoster/TurnMonsterFsm.h"

void UUiTurn::NativeConstruct()
{
	Super::NativeConstruct();

	AGameStateBase* GameStateBase = UGameplayStatics::GetGameState(this);
	Gs = Cast<AGameStateOpen>(GameStateBase);

	Button_TurnStart->OnClicked.AddDynamic(this,&UUiTurn::OnTurnStart);
	Button_TurnComplete->OnClicked.AddDynamic(this,&UUiTurn::OnTurnComplete);
}

void UUiTurn::OnTurnStart()
{
	Gs->FindPlayerTurn();
}

void UUiTurn::OnTurnComplete()
{
	Gs->FindPlayerTurn();
	Gs->FindMonsterTurn();

	GetWorld()->GetTimerManager().SetTimer(TurnTimerHandle,[this]()
	{
		TimerSecond += GetWorld()->GetDeltaSeconds();
		if (TimerSecond > 2)
		{
			Gs->TurnMonsters[0]->TurnMonsterFsm->MoveToPlayer();
			GetWorld()->GetTimerManager().ClearTimer(TurnTimerHandle);
		}
	},0.02f,true);
}

