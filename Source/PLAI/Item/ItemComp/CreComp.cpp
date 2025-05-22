// Fill out your copyright notice in the Description page of Project Settings.


#include "CreComp.h"

#include "PLAI/Item/Creture/Creature.h"
#include "PLAI/Item/Creture/Dragon/CreDragon.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"


// Sets default values for this component's properties
UCreComp::UCreComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCreComp::BeginPlay()
{
	Super::BeginPlay();
	TestPlayer = Cast<ATestPlayer>(GetOwner());
	if (!TestPlayer)
	{ UE_LOG(LogTemp,Warning,TEXT("UCreComp::BeginPlay TestPlayer캐스팅 실패"))}
	{ PC = Cast<APlayerController>(TestPlayer->GetController());
		if (!PC)
		{
			UE_LOG(LogTemp,Warning,TEXT("UCreComp::BeginPlay PC 캐스팅 실패"));
		}
	}
}


// Called every frame
void UCreComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCreComp::EquipCreature(ACreature* SpawnCreature)
{
	if (!SpawnCreature)
	{
		UE_LOG(LogTemp,Warning,TEXT("CreComp CretureFactory없음")); return;
	}
	UE_LOG(LogTemp,Warning,TEXT("CreComp CretureFactory있음"))

	Creature = SpawnCreature;
	Creature->AttachToActor(TestPlayer,FAttachmentTransformRules::KeepRelativeTransform);
	Creature->SetActorLocation(TestPlayer->GetActorLocation()+FVector(0,125,125));
	TestPlayer->SetOwner(Creature);
}
