// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/BattlePlayer/Player/BattlePlayer.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void ABattlePlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (const auto* pc = Cast<APlayerController>(Controller)) {
		//그 객체를 이용해서 EnhanceInputLocalPlayerSubSystem을 가져온다.
		UEnhancedInputLocalPlayerSubsystem* subSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subSys) {
			subSys->AddMappingContext(IMC_Player, 0);
		}
	}
}

void ABattlePlayer::SetupPlayerInputComponent(
	UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* pi = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		pi->BindAction(IA_Move, ETriggerEvent::Completed, this, &ABaseBattlePawn::MouseClick);
	}
}
