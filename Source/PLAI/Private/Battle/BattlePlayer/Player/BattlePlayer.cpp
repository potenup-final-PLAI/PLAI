// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/BattlePlayer/Player/BattlePlayer.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BattlePlayerAnimInstance.h"

ABattlePlayer::ABattlePlayer()
{
	// 플레이어 세팅
	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetRelativeLocationAndRotation(FVector(0, 0, -100), FRotator(0, -90, 0));
	
	// Mesh Setting
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonGreystone/Characters/Heroes/Greystone/Meshes/Greystone.Greystone'"));
	if (tempMesh.Succeeded())
	{
		meshComp->SetSkeletalMesh(tempMesh.Object);
	}
	
	// Animation Instance 세팅
	ConstructorHelpers::FClassFinder<UAnimInstance> tempAnimInstance(TEXT("/Script/Engine.AnimBlueprint'/Game/JS/Blueprints/Animation/ABP_BattlePlayer.ABP_BattlePlayer_C'"));
	if (tempAnimInstance.Succeeded())
	{
		meshComp->SetAnimInstanceClass(tempAnimInstance.Class);
	}
}

void ABattlePlayer::BeginPlay()
{
	Super::BeginPlay();
	
	// GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &ABaseBattlePawn::TryInitStatus, 0.1f, true);
}

void ABattlePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::SpaceBar))
	{
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("Mk_LevelVillage"));
	}
}

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
	playerAnim = Cast<UBattlePlayerAnimInstance>(meshComp->GetAnimInstance());
}

void ABattlePlayer::SetupPlayerInputComponent(
	UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* pi = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		pi->BindAction(IA_Move, ETriggerEvent::Completed, this, &ABaseBattlePawn::OnMouseLeftClick);
	}
}


