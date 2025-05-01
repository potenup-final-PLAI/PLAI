// Fill out your copyright notice in the Description page of Project Settings.


#include "TestPlayer.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/SpringArmComponent.h"
#include "PLAI/Item/ItemComp/CreComp.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/ItemComp/ItemComp.h"
#include "PLAI/Item/ItemComp/StoreComp.h"
#include "PLAI/Item/Login/LoginComp.h"
#include "PLAI/Item/Login/LogItemComp.h"
#include "PLAI/Item/UI/Inventory/StoreInven/StoreInven.h"


// Sets default values
ATestPlayer::ATestPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	InvenComp = CreateDefaultSubobject<UInvenComp>(TEXT("InvenComp"));
	StoreComp = CreateDefaultSubobject<UStoreComp>(TEXT("StoreComp"));
	LoginComp = CreateDefaultSubobject<ULoginComp>(TEXT("LoginComp"));
	CreComp = CreateDefaultSubobject<UCreComp>(TEXT("CreComp"));
	LogItemComp = CreateDefaultSubobject<ULogItemComp>(TEXT("ItemComp"));

	CaptureComp = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComp"));
	CaptureComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATestPlayer::BeginPlay()
{
	Super::BeginPlay();
	StoreComp->StoreInven->AddToViewport();
	CaptureComp->PrimaryComponentTick.bCanEverTick = true;
    
	
	// if (APlayerController* Pc = Cast<APlayerController>(GetController()))
	// {
	// 	TopDownCameraComponent->SetActive(false);
	// 	Pc->SetViewTarget(this);
	// }
	// //카메라붐 카메라 회전값적용
	// CameraBoom->bUsePawnControlRotation = true;
}

// Called every frame
void ATestPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATestPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

