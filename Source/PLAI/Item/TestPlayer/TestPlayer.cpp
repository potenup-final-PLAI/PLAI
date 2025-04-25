// Fill out your copyright notice in the Description page of Project Settings.


#include "TestPlayer.h"

#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/ItemComp/ItemComp.h"
#include "PLAI/Item/ItemComp/StoreComp.h"
#include "PLAI/Item/Login/LoginComp.h"
#include "PLAI/Item/UI/Inventory/StoreInven/StoreInven.h"


// Sets default values
ATestPlayer::ATestPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	InvenComp = CreateDefaultSubobject<UInvenComp>(TEXT("InvenComp"));
	StoreComp = CreateDefaultSubobject<UStoreComp>(TEXT("StoreComp"));
	LoginComp = CreateDefaultSubobject<ULoginComp>(TEXT("LoginComp"));
}

// Called when the game starts or when spawned
void ATestPlayer::BeginPlay()
{
	Super::BeginPlay();
	StoreComp->StoreInven->AddToViewport();
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

