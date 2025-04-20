// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/UI/ActionUI.h"

#include "Battle/TurnSystem/PhaseManager.h"
#include "Components/Button.h"

void UActionUI::NativeConstruct()
{
	Super::NativeConstruct();

	btn_TurnEnd->OnClicked.AddDynamic(this, &UActionUI::OnClickedTurnEnd);
	btn_Move->OnClicked.AddDynamic(this, &UActionUI::OnClickedMove);
	btn_FirstSkill->OnClicked.AddDynamic(this, &UActionUI::OnClickedFirstSkill);
	btn_SecondSkill->OnClicked.AddDynamic(
		this, &UActionUI::OnClickedSecondSkill);
}

void UActionUI::OnClickedTurnEnd()
{
	auto* PhaseManager = Cast<AUPhaseManager>(GetWorld()->GetGameState());
	if (PhaseManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("Click Turn End"));
		PhaseManager->EndPlayerPhase();
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
