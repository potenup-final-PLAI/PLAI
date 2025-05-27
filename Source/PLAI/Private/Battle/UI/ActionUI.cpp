// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/UI/ActionUI.h"

#include "Battle/TurnSystem/PhaseManager.h"
#include "Components/Button.h"
#include "Enemy/BaseEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BattlePlayer.h"

void UActionUI::NativeConstruct()
{
	Super::NativeConstruct();

	phaseManager = Cast<AUPhaseManager>(GetWorld()->GetGameState());
	turnManager = Cast<ATurnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), turnManagerFactory));

	// UI 바인딩
	btn_TurnEnd->OnClicked.AddDynamic(this, &UActionUI::OnClickedTurnEnd);
	btn_Move->OnClicked.AddDynamic(this, &UActionUI::OnClickedMove);
	// skill 부분
	btn_BaseAttack->OnClicked.AddDynamic(this, &UActionUI::OnClickedBaseAttack);
	btn_Poison->OnClicked.AddDynamic(this, &UActionUI::OnClickedPoison);
	btn_Fatal->OnClicked.AddDynamic(this, &UActionUI::OnClickedFatal);
	btn_Rupture->OnClicked.AddDynamic(this, &UActionUI::OnClickedRupture);
}

void UActionUI::OnClickedTurnEnd()
{
	auto* player = IsMyTurn();
	if (!player)
	{
		// 플레이어가 아니니까 return
		UE_LOG(LogTemp, Warning, TEXT("Block!!! Is Not Player"));
		return;
	}

	player->Server_OnClickedTurnEnd();
}

void UActionUI::OnClickedMove()
{
	auto* player = IsMyTurn();
	if (!player)
	{
		// 플레이어가 아니니까 return
		UE_LOG(LogTemp, Warning, TEXT("Block!!! Is Not Player"));
		return;
	}
	
	player->Server_OnClickedMove();
}

void UActionUI::OnClickedBaseAttack()
{
	auto* player = IsMyTurn();
	if (!player)
	{
		// 플레이어가 아니니까 return
		UE_LOG(LogTemp, Warning, TEXT("Block!!! Is Not Player"));
		return;
	}

	player->Server_OnClickedBaseAttack();


}

void UActionUI::OnClickedPoison()
{
	auto* player = IsMyTurn();
	if (!player)
	{
		// 플레이어가 아니니까 return
		UE_LOG(LogTemp, Warning, TEXT("Block!!! Is Not Player"));
		return;
	}

	player->Server_OnClickedPoison();
}

void UActionUI::OnClickedFatal()
{
	auto* player = IsMyTurn();
	if (!player)
	{
		// 플레이어가 아니니까 return
		UE_LOG(LogTemp, Warning, TEXT("Block!!! Is Not Player"));
		return;
	}

	player->Server_OnClickedFatal();

}

void UActionUI::OnClickedRupture()
{
	auto* player = IsMyTurn();
	if (!player)
	{
		// 플레이어가 아니니까 return
		UE_LOG(LogTemp, Warning, TEXT("Block!!! Is Not Player"));
		return;
	}

	player->Server_OnClickedRupture();
}

ABattlePlayer* UActionUI::IsMyTurn()
{
	auto* player = Cast<ABattlePlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (turnManager->curUnit && player->GetUniqueID() == turnManager->curUnit->GetUniqueID())
	{
		return player;
	}
	return nullptr;
}
