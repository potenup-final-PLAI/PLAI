// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenPlayer.h"

#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/ItemComp/StoreComp.h"
#include "PLAI/Item/Login/LoginComp.h"


// Sets default values
AOpenPlayer::AOpenPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	InvenComp = CreateDefaultSubobject<UInvenComp>(TEXT("InvenComp"));
	StoreComp = CreateDefaultSubobject<UStoreComp>(TEXT("StoreComp"));
	LoginComp = CreateDefaultSubobject<ULoginComp>(TEXT("LoginComp"));
}

// Called when the game starts or when spawned
void AOpenPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOpenPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AOpenPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

