// Fill out your copyright notice in the Description page of Project Settings.


#include "TestPlayer.h"

#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputComp/InputComp.h"
#include "PLAI/Item/GameInstance/WorldGi.h"
#include "PLAI/Item/ItemComp/CreComp.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/ItemComp/ItemComp.h"
#include "PLAI/Item/ItemComp/StoreComp.h"
#include "PLAI/Item/Login/LoginComp.h"
#include "PLAI/Item/Login/LogItemComp.h"
#include "PLAI/Item/UI/Inventory/Map/UiWorldMap.h"
#include "PLAI/Item/UI/Inventory/Quest/UiQuest.h"
#include "PLAI/Item/UI/Inventory/StoreInven/StoreInven.h"
#include "TurnComp/TurnComp.h"


// Sets default values
ATestPlayer::ATestPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	GetCharacterMovement()->SetIsReplicated(true);
	
	InvenComp = CreateDefaultSubobject<UInvenComp>(TEXT("InvenComp"));
	StoreComp = CreateDefaultSubobject<UStoreComp>(TEXT("StoreComp"));
	LoginComp = CreateDefaultSubobject<ULoginComp>(TEXT("LoginComp"));
	CreComp = CreateDefaultSubobject<UCreComp>(TEXT("CreComp"));
	LogItemComp = CreateDefaultSubobject<ULogItemComp>(TEXT("ItemComp"));
	InputComp = CreateDefaultSubobject<UInputComp>(TEXT("TestInputComp"));
	TurnComp = CreateDefaultSubobject<UTurnComp>(TEXT("TurnComp"));
	
	CaptureComp = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComp"));
	CaptureComp->SetupAttachment(RootComponent);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>("NIagaraComp");
	// NiagaraComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATestPlayer::BeginPlay()
{
	Super::BeginPlay();
    if (IsLocallyControlled())
    {
    	CaptureComp->PrimaryComponentTick.bCanEverTick = true;
    }
	TestPlayerParent = TestPlayerFactory->GetDefaultObject<ATestPlayer>();

	SetActorScale3D(FVector(2.25f));
}

// Called every frame
void ATestPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentTime += GetWorld()->GetDeltaSeconds();
	
	if (CurrentTime > 1) { CurrentTime = 0; }
	
	RotateTime += GetWorld()->GetDeltaSeconds();
	if (RotateTime > 2) { RotateTime = 0; }

	if (FVector::Dist(GetActorLocation(), QuestLocation) < 1500)
	{
		NiagaraComp->SetWorldLocation(FVector(0,0,200)+QuestLocation + 250*FRotator(0,RotateTime*180,0).Vector() );
	}
	else
	{
		NiagaraComp->SetWorldRotation((QuestLocation-GetActorLocation()).GetSafeNormal().Rotation());
		NiagaraComp->SetWorldLocation(GetActorLocation() + (QuestLocation-GetActorLocation()).GetSafeNormal() * (600 + 600 * CurrentTime));
	}
	
}

// Called to bind functionality to input
void ATestPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
