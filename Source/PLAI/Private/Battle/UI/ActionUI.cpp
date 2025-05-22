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
	turnManager = Cast<ATurnManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), turnManagerFactory));

	// UI 바인딩
	btn_TurnEnd->OnClicked.AddDynamic(this, &UActionUI::OnClickedTurnEnd);
	btn_Move->OnClicked.AddDynamic(this, &UActionUI::OnClickedMove);
	// skill 부분
	btn_BaseAttack->OnClicked.AddDynamic(this, &UActionUI::OnClickedBaseAttack);
	btn_Paralysis->OnClicked.AddDynamic(this, &UActionUI::OnClickedParalysis);
	btn_Poison->OnClicked.AddDynamic(this, &UActionUI::OnClickedPoison);
	btn_Vulnerable->OnClicked.AddDynamic(this, &UActionUI::OnClickedVulnerable);
	btn_Weakening->OnClicked.AddDynamic(this, &UActionUI::OnClickedWeakening);
	btn_Fatal->OnClicked.AddDynamic(this, &UActionUI::OnClickedFatal);
	btn_Rupture->OnClicked.AddDynamic(this, &UActionUI::OnClickedRupture);
	btn_Roar->OnClicked.AddDynamic(this, &UActionUI::OnClickedRoar);
	btn_BattleCry->OnClicked.AddDynamic(this, &UActionUI::OnClickedBattleCry);
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
		UE_LOG(LogTemp, Warning,
		       TEXT("ActionUI OnClickedTurnEnd phaseManager nullPtr"));
		return;
	}
	// 버튼 상태 업데이트
	turnManager->curUnit->currentActionMode = EActionMode::TurnEnd;

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
	if (!(Cast<ABattlePlayer>(turnManager->curUnit)))
	{
		// 플레이어가 아니니까 return
		UE_LOG(LogTemp, Warning, TEXT("Block!!! Is Not Player"));
		return;
	}
	if (!turnManager->curUnit->IsLocallyControlled())
	{
		UE_LOG(LogTemp, Warning, TEXT("not LocallyControlled"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Move"));
	turnManager->curUnit->currentActionMode = EActionMode::Move;
	turnManager->curUnit->bIsMoveMode = true;
	// 범위 보이게 설정
	turnManager->curUnit->ServerRPC_SeeMoveRange(turnManager->curUnit->moveRange);
	UE_LOG(LogTemp, Warning, TEXT("move_Range %d"), turnManager->curUnit->moveRange);
}

void UActionUI::OnClickedBaseAttack()
{
	if (!(Cast<ABattlePlayer>(turnManager->curUnit)))
	{
		// 플레이어가 아니니까 return
		UE_LOG(LogTemp, Warning, TEXT("Block!!! Is Not Player"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("FirstSkill"));
	turnManager->curUnit->currentActionMode = EActionMode::BaseAttack;
}

void UActionUI::OnClickedParalysis()
{
	if (!(Cast<ABattlePlayer>(turnManager->curUnit)))
	{
		// 플레이어가 아니니까 return
		UE_LOG(LogTemp, Warning, TEXT("Block!!! Is Not Player"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("SecondSkill"));
	turnManager->curUnit->currentActionMode = EActionMode::Paralysis;
}

void UActionUI::OnClickedPoison()
{
	if (!(Cast<ABattlePlayer>(turnManager->curUnit)))
	{
		// 플레이어가 아니니까 return
		UE_LOG(LogTemp, Warning, TEXT("Block!!! Is Not Player"));
		return;
	}
	turnManager->curUnit->currentActionMode = EActionMode::Poison;
}

void UActionUI::OnClickedVulnerable()
{
	if (!(Cast<ABattlePlayer>(turnManager->curUnit)))
	{
		// 플레이어가 아니니까 return
		UE_LOG(LogTemp, Warning, TEXT("Block!!! Is Not Player"));
		return;
	}
	turnManager->curUnit->currentActionMode = EActionMode::Vulnerable;
}

void UActionUI::OnClickedWeakening()
{
	if (!(Cast<ABattlePlayer>(turnManager->curUnit)))
	{
		// 플레이어가 아니니까 return
		UE_LOG(LogTemp, Warning, TEXT("Block!!! Is Not Player"));
		return;
	}
	turnManager->curUnit->currentActionMode = EActionMode::Weakening;
}

void UActionUI::OnClickedFatal()
{
	if (!(Cast<ABattlePlayer>(turnManager->curUnit)))
	{
		// 플레이어가 아니니까 return
		UE_LOG(LogTemp, Warning, TEXT("Block!!! Is Not Player"));
		return;
	}
	turnManager->curUnit->currentActionMode = EActionMode::Fatal;
}

void UActionUI::OnClickedRupture()
{
	if (!(Cast<ABattlePlayer>(turnManager->curUnit)))
	{
		// 플레이어가 아니니까 return
		UE_LOG(LogTemp, Warning, TEXT("Block!!! Is Not Player"));
		return;
	}
	turnManager->curUnit->currentActionMode = EActionMode::Rupture;
}

void UActionUI::OnClickedRoar()
{
	if (!(Cast<ABattlePlayer>(turnManager->curUnit)))
	{
		// 플레이어가 아니니까 return
		UE_LOG(LogTemp, Warning, TEXT("Block!!! Is Not Player"));
		return;
	}
	turnManager->curUnit->currentActionMode = EActionMode::Roar;
}

void UActionUI::OnClickedBattleCry()
{
	if (!(Cast<ABattlePlayer>(turnManager->curUnit)))
	{
		// 플레이어가 아니니까 return
		UE_LOG(LogTemp, Warning, TEXT("Block!!! Is Not Player"));
		return;
	}
	turnManager->curUnit->currentActionMode = EActionMode::BattleCry;
}
