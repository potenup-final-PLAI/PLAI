// Fill out your copyright notice in the Description page of Project Settings.


#include "UiTurn.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "PLAI/Item/GameState/GameStateOpen.h"

void UUiTurn::NativeConstruct()
{
	Super::NativeConstruct();

	AGameStateBase* GameStateBase = UGameplayStatics::GetGameState(this);
	Gs = Cast<AGameStateOpen>(GameStateBase);

	Button_TurnStart->OnClicked.AddDynamic(this,&UUiTurn::OnTurnStart);
}

void UUiTurn::OnTurnStart()
{
	Gs->FindPlayerTurn();
}
