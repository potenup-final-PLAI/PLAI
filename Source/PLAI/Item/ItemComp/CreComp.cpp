// Fill out your copyright notice in the Description page of Project Settings.


#include "CreComp.h"

#include "PLAI/Item/Creture/Creature.h"
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

	// if (TestPlayer->HasAuthority() && PC->WasInputKeyJustPressed(EKeys::Five))
	// {
	// 	if (TestPlayer->IsLocallyControlled())
	// 	{
	// 		Creature = GetWorld()->SpawnActor<ACreature>(CreatureFactory);
	// 		Creature->AttachToActor(TestPlayer,FAttachmentTransformRules::KeepRelativeTransform);
	// 		Creature->SetActorLocation(TestPlayer->GetActorLocation() + FVector(120,-120,120));
	// 	}
	// }

	// ...
}

