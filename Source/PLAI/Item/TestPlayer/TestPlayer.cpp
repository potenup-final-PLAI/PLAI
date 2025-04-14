// Fill out your copyright notice in the Description page of Project Settings.


#include "TestPlayer.h"

#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/ItemComp/ItemComp.h"


// Sets default values
ATestPlayer::ATestPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	InvenComp = CreateDefaultSubobject<UInvenComp>(TEXT("InvenComp"));
}

// Called when the game starts or when spawned
void ATestPlayer::BeginPlay()
{
	Super::BeginPlay();
	// InvenComp = Cast<UItemComp>(InvenComp);
	// if (InvenComp)
	// {
	// 	UE_LOG(LogTemp,Warning,TEXT("캐릭터 InvenComp캐스팅성공"));
	// }
	// else
	// {
	// 	UE_LOG(LogTemp,Warning,TEXT("캐릭터 InvenComp캐스팅실패"));
	// }
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

