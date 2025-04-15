// Copyright Epic Games, Inc. All Rights Reserved.

#include "PLAI/Public/BaseFile/PLAIGameMode.h"
#include "PLAI/Public/BaseFile/PLAIPlayerController.h"
#include "UObject/ConstructorHelpers.h"

APLAIGameMode::APLAIGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = APLAIPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}