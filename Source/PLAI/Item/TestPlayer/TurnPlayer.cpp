// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnPlayer.h"

#include "AIController.h"
#include "Components/WidgetComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "PLAI/Item/GameState/GameStateOpen.h"
#include "PLAI/Item/Login/LoginComp.h"
#include "PLAI/Item/UI/Turn/UITurnHpBar.h"


class AAIController;
// Sets default values
ATurnPlayer::ATurnPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	WidgetComp = CreateDefaultSubobject<UWidgetComponent>("Widget");
	WidgetComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATurnPlayer::BeginPlay()
{
	Super::BeginPlay();

	GameState = Cast<AGameStateOpen>(GetWorld()->GetGameState());

	UITurnHpBar = CreateWidget<UUITurnHpBar>(GetWorld(),TurnHpBarFactory);
	UITurnHpBar->SetHpBar(TurnPlayerStruct);

	WidgetComp->SetWidget(UITurnHpBar);
	WidgetComp->SetDrawSize(FVector2D(200.f, 50.f));
	WidgetComp->SetVisibility(true);
	WidgetComp->SetRelativeLocation(FVector(0,0,200));
	WidgetComp->SetWidgetSpace(EWidgetSpace::Screen); // or World
}

// Called every frame
void ATurnPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugString(GetWorld(),GetActorLocation()+FVector(0,0,150),FString::Printf(TEXT("나의 턴 있음?? [%i] \n"
	"내턴은 몇번째? [%d]"),bTurn,TurnIndex),
		nullptr,FColor::Red,0,true,1);

	// DrawDebugString(GetWorld(),TestPlayer->GetActorLocation() + FVector(0, 0, 150),
	// FString::Printf(TEXT("CreDreFsm DraState 현재[%s]"),*UEnum::GetValueAsString(EDraState(static_cast<int32>
	// 	(Drastate)))),nullptr,FColor::Red,0.f,false);
}

// Called to bind functionality to input
void ATurnPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATurnPlayer::PlayerState(FVector Location, ATurnMonster* TurnMonster)
{
	switch (TurnPlayerState)
	{
	case ETurnPlayerState::Idle:
		Idle();
		break;
		
	case ETurnPlayerState::Move:
		MoveToMonster(Location);
		break;
		
	case ETurnPlayerState::Attack:
		AttackToMonster(TurnMonster);
		break;
		
	case ETurnPlayerState::Avoid:
		AvoidToMonster();
		break;
	}
}

void ATurnPlayer::Idle()
{
}

void ATurnPlayer::MoveToMonster(FVector Location)
{
	UE_LOG(LogTemp,Warning,TEXT("ATurnPlayer::MoveToMonster 실행이 되고있니 위치는? [%s]"),*Location.ToString());
	AAIController* AI = GetWorld()->SpawnActor<AAIController>(AIControllerClass);
	AI->Possess(this);
	
	AI->MoveToLocation(Location,100, true,true,true);
	
	AI->ReceiveMoveCompleted.AddUniqueDynamic(this,&ATurnPlayer::OnAIMoveCompleted);
}

void ATurnPlayer::AttackToMonster(ATurnMonster* TurnMonster)
{
	if (!TurnMonster)return;
	UE_LOG(LogTemp,Warning,TEXT("TurnPlayer TurnMonster 공격"))
	TurnMonster->TurnMonsterStruct.CurrentHp -= TurnPlayerStruct.Atk;
	TurnMonster->SetMonsterUi();
}

void ATurnPlayer::AvoidToMonster()
{
}

void ATurnPlayer::OnAIMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result == EPathFollowingResult::Type::Success)
	{
		UE_LOG(LogTemp,Warning,TEXT("TurnPlayer TurnMonster 위치 도착 공격State전환"))
		TurnPlayerState = ETurnPlayerState::Attack;
	}
}


