// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterMaster.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "MonUi/MonUi.h"


// Sets default values
AMonsterMaster::AMonsterMaster()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMonsterMaster::BeginPlay()
{
	Super::BeginPlay();
	// 부모 DefaultObject에서 가져오기
}

void AMonsterMaster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Called every frame
void AMonsterMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input

