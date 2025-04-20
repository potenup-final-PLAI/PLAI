// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcStart.h"


// Sets default values
ANpcStart::ANpcStart()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANpcStart::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ANpcStart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ANpcStart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ANpcStart::WarriorStarter()
{
	FItemStruct WarriorWeaponStruct;
	
	WarriorWeaponStruct.ItemTop = 1;
	WarriorWeaponStruct.ItemIndex = 0;
	WarriorWeaponStruct.ItemIndexType = 0;
	WarriorWeaponStruct.ItemIndexDetail = 0;
	
	ItemStructsArray.ItemStructs.Add(WarriorWeaponStruct);

	FItemStruct WarriorArmorStruct;
	
	WarriorArmorStruct.ItemTop = 1;
	WarriorArmorStruct.ItemIndex = 1;
	WarriorArmorStruct.ItemIndexType = 0;
	WarriorArmorStruct.ItemIndexDetail = 0;

	ItemStructsArray.ItemStructs.Add(WarriorArmorStruct);
}


void ANpcStart::HunterStarter()
{
	// 헌터셋팅
	FItemStruct HunterWeaponStruct;
	
	HunterWeaponStruct.ItemTop = 1;
	HunterWeaponStruct.ItemIndex = 0;
	HunterWeaponStruct.ItemIndexType = 1;
	HunterWeaponStruct.ItemIndexDetail = 0;
	
	ItemStructsArray.ItemStructs.Add(HunterWeaponStruct);

	FItemStruct HunterArmorStruct;
	
	HunterArmorStruct.ItemTop = 1;
	HunterArmorStruct.ItemIndex = 1;
	HunterArmorStruct.ItemIndexType = 0;
	HunterArmorStruct.ItemIndexDetail = 0;

	ItemStructsArray.ItemStructs.Add(HunterArmorStruct);

	
}


