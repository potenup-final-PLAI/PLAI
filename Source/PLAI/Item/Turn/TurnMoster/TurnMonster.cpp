// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnMonster.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/ProgressBar.h"
#include "Components/SphereComponent.h"
#include "Components/TextBlock.h"
#include "PLAI/Item/Monster/MonUi/MonUi.h"
#include "Slate/SGameLayerManager.h"


// Sets default values
ATurnMonster::ATurnMonster()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATurnMonster::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATurnMonster::Tick(float DeltaTime)
{
	ACharacter::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATurnMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATurnMonster::SetMonsterUi()
{
	MonUi->Name->SetText(FText::FromString(TurnMonsterStruct.Name));
	MonUi->CurrentHp->SetText(FText::AsNumber(TurnMonsterStruct.CurrentHp));
	MonUi->MaxHp->SetText(FText::AsNumber(TurnMonsterStruct.MaxHp));
}

void ATurnMonster::SetHpBar()
{
	MonUi->HpBar->SetPercent(static_cast<float>(TurnMonsterStruct.CurrentHp) / TurnMonsterStruct.MaxHp);
	MonUi->CurrentHp->SetText(FText::AsNumber(TurnMonsterStruct.CurrentHp));
}

void ATurnMonster::MoveToMonster()
{
	UE_LOG(LogTemp,Warning,TEXT("ATurnMonster::MoveToMonster 실행이 되고있니"));
	AAIController* AI = GetWorld()->SpawnActor<AAIController>(AIControllerClass);
	AI->Possess(this);
	AI->MoveToLocation(GetActorForwardVector() * 250,25, true,false,false);
}