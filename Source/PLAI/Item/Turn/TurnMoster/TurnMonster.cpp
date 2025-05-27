// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnMonster.h"

#include "AIController.h"
#include "TurnMonsterFsm.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/ProgressBar.h"
#include "Components/SphereComponent.h"
#include "Components/TextBlock.h"
#include "PLAI/Item/GameState/GameStateOpen.h"
#include "PLAI/Item/Monster/MonUi/MonUi.h"
#include "Slate/SGameLayerManager.h"


// Sets default values
ATurnMonster::ATurnMonster()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TurnMonsterFsm = CreateDefaultSubobject<UTurnMonsterFsm>(TEXT("TurnMonsterFsm"));
}

// Called when the game starts or when spawned
void ATurnMonster::BeginPlay()
{
	Super::BeginPlay();

	AiController = GetWorld()->SpawnActor<AAIController>();
	AiController->Possess(this);

	GameState = Cast<AGameStateOpen>(GetWorld()->GetGameState());
}

// Called every frame
void ATurnMonster::Tick(float DeltaTime)
{
	ACharacter::Tick(DeltaTime);

	DrawDebugString(GetWorld(),GetActorLocation()+FVector(0,0,150),FString::Printf(TEXT("나의 턴 있음?? [%i] \n"
	"내턴은 몇번째? [%d]"),bTurn,TurnIndex),nullptr,FColor::Red,0,true,1);
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
	MonUi->HpBar->SetPercent(static_cast<float>(TurnMonsterStruct.CurrentHp) / TurnMonsterStruct.MaxHp);
	MonUi->CurrentHp->SetText(FText::AsNumber(TurnMonsterStruct.CurrentHp));
}

void ATurnMonster::SetHpBar()
{
	MonUi->HpBar->SetPercent(static_cast<float>(TurnMonsterStruct.CurrentHp) / TurnMonsterStruct.MaxHp);
	MonUi->CurrentHp->SetText(FText::AsNumber(TurnMonsterStruct.CurrentHp));
}