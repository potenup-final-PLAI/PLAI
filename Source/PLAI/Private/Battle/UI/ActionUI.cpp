// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/UI/ActionUI.h"

#include "Battle/TurnSystem/PhaseManager.h"
#include "Components/Button.h"
#include "Developer/AITestSuite/Public/AITestsCommon.h"
#include "Enemy/BaseEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BattlePlayer.h"

void UActionUI::NativeConstruct()
{
	Super::NativeConstruct();

	phaseManager = Cast<AUPhaseManager>(GetWorld()->GetGameState());
	turnManager = Cast<ATurnManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), turnManagerFactory));

	// UI 바인딩
	btn_TurnEnd->OnClicked.AddDynamic(this, &UActionUI::OnClickedTurnEnd);
	btn_Move->OnClicked.AddDynamic(this, &UActionUI::OnClickedMove);
	btn_FirstSkill->OnClicked.AddDynamic(this, &UActionUI::OnClickedFirstSkill);
	btn_SecondSkill->OnClicked.AddDynamic(
		this, &UActionUI::OnClickedSecondSkill);
}

void UActionUI::OnClickedTurnEnd()
{
	if (!(Cast<ABattlePlayer>(turnManager->curUnit)))
	{
		// 플레이어가 아니니까 return
		UE_LOG(LogTemp, Warning, TEXT("Block!!! Is Not Player"));
		return;
	}
	if (phaseManager->currentPhase == EBattlePhase::BattleEnd)
	{
		UE_LOG(LogTemp, Warning, TEXT("Block!!! Battle End"));
		return;
	}
	if (phaseManager == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ActionUI OnClickedTurnEnd phaseManager nullPtr"));
		return;
	}
	if (ABattlePlayer* player = Cast<ABattlePlayer>(turnManager->curUnit))
	{
		// 플레이어 턴 종료
		UE_LOG(LogTemp, Warning, TEXT("Click Turn End"));
		player->OnTurnEnd();
	}
	else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(turnManager->curUnit))
	{
		// Enemy 턴 종료
		UE_LOG(LogTemp, Warning, TEXT("Click Turn End"));
		enemy->OnTurnEnd();
	}
}

void UActionUI::OnClickedMove()
{
	UE_LOG(LogTemp, Warning, TEXT("Move"));
}

void UActionUI::OnClickedFirstSkill()
{
	UE_LOG(LogTemp, Warning, TEXT("FirstSkill"));
}

void UActionUI::OnClickedSecondSkill()
{
	UE_LOG(LogTemp, Warning, TEXT("SecondSkill"));
}
