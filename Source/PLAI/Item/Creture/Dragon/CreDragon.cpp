// Fill out your copyright notice in the Description page of Project Settings.


#include "CreDragon.h"

#include "CreDraFsm.h"


// Sets default values
ACreDragon::ACreDragon()
{
	CreFsm = CreateDefaultSubobject<UCreDraFsm>("CreFsm");
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACreDragon::BeginPlay()
{
	Super::BeginPlay();
	CreFsm = Cast<UCreDraFsm>(CreFsm);
	
	if (CreFsm)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreDragon CreFsm -> DraFSm 성공"));
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("CreDragon CreFsm -> DraFSm 실패"));
	}
}

// Called every frame
void ACreDragon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACreDragon::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

