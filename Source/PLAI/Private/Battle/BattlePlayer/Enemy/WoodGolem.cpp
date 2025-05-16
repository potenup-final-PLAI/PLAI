// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/WoodGolem.h"


// Sets default values
AWoodGolem::AWoodGolem()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Mesh Setting
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("'/Game/Wood_Monster/CharacterParts/Meshes/SK_wood_giant_01_a.SK_wood_giant_01_a'"));
	if (tempMesh.Succeeded())
	{
		meshComp->SetSkeletalMesh(tempMesh.Object);
	}

	// Animation Instance 세팅
	// ConstructorHelpers::FClassFinder<UAnimInstance> tempAnimInstance(TEXT("'/Game/JS/Blueprints/Animation/ABP_BattleEnemy.ABP_BattleEnemy_C'"));
	// if (tempAnimInstance.Succeeded())
	// {
	// 	meshComp->SetAnimInstanceClass(tempAnimInstance.Class);
	// }
}

// Called when the game starts or when spawned
void AWoodGolem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWoodGolem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AWoodGolem::SetupPlayerInputComponent(
	UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

