// Fill out your copyright notice in the Description page of Project Settings.


#include "KHA/KHACharacter.h"

#include "NPC.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Evaluation/Blending/MovieSceneBlendType.h"
#include "Engine/EngineTypes.h"
#include "Engine/OverlapResult.h"

// Sets default values
AKHACharacter::AKHACharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AKHACharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKHACharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKHACharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Interact",IE_Pressed, this, &AKHACharacter::TryInteract);
	
}

void AKHACharacter::TryInteract()
{
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	FVector MyLocation = GetActorLocation();
	float DetectRadius = 300.f;

	DrawDebugSphere(GetWorld(), MyLocation, DetectRadius, 12, FColor::Green, false, 1.0f);

	bool bHit = GetWorld()->OverlapMultiByChannel(
		Overlaps,
		MyLocation,
		FQuat::Identity,
		ECC_Pawn, // NPC는 Pawn이니까
		FCollisionShape::MakeSphere(DetectRadius),
		Params
	);

	if (bHit)
	{
		for (auto& Result : Overlaps)
		{
			if (ANPC* NPC = Cast<ANPC>(Result.GetActor()))
			{
				if (NPC->IsPlayerInRange())
				{
					NPC->InteractTurnBased(this);
					return;
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("주변에 NPC 없음 또는 범위 밖"));
}


