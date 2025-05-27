// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnPlayer.h"

#include "AIController.h"


class AAIController;
// Sets default values
ATurnPlayer::ATurnPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATurnPlayer::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATurnPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AddMovementInput(FVector(0.0f, 0.0f, -10.0f));
}

// Called to bind functionality to input
void ATurnPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATurnPlayer::MoveToPlayer()
{
	if (MoveLocation == FVector::ZeroVector) return;
	
	UE_LOG(LogTemp,Warning,TEXT("ATurnPlayer::MoveToMonster 실행이 되고있니"));
	AAIController* AI = GetWorld()->SpawnActor<AAIController>(AIControllerClass);
	AI->Possess(this);
	AI->MoveToLocation(MoveLocation,25, true,true,false);
}
