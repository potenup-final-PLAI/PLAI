// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnPlayer.h"

#include "AIController.h"
#include "PLAI/Item/Login/LoginComp.h"


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
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
	{ if (LoginComp->UiMain) { LoginComp->UiMain->RemoveFromParent(); } },0.5,false);
}

// Called every frame
void ATurnPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugString(GetWorld(),GetActorLocation()+FVector(0,0,150),FString::Printf(TEXT("나의 턴 있음?? [%i] \n"
	"내턴은 몇번째? [%d]"),bTurn,TurnIndex),
		nullptr,FColor::Blue,0,false);

	// DrawDebugString(GetWorld(),TestPlayer->GetActorLocation() + FVector(0, 0, 150),
	// FString::Printf(TEXT("CreDreFsm DraState 현재[%s]"),*UEnum::GetValueAsString(EDraState(static_cast<int32>
	// 	(Drastate)))),nullptr,FColor::Red,0.f,false);
}

// Called to bind functionality to input
void ATurnPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATurnPlayer::MoveToPlayer()
{
	if (MoveLocation == FVector::ZeroVector) return;
	
	UE_LOG(LogTemp,Warning,TEXT("ATurnPlayer::MoveToMonster 실행이 되고있니"));
	AAIController* AI = GetWorld()->SpawnActor<AAIController>(AIControllerClass);
	AI->Possess(this);
	AI->MoveToLocation(MoveLocation,25, true,true,true);
}
