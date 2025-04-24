// Fill out your copyright notice in the Description page of Project Settings.


#include "KHA/KHACharacter.h"

#include "MonsterCharacter.h"
#include "NPC.h"
#include "NPC2.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
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
#include "Kismet/GameplayStatics.h"

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

	if (bShouldMove)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(PC, LastClickLocation);
			bShouldMove = false; // 한 번만 실행
		}
	}

	if (CameraBoom)
	{
		float TargetZoom = bZoomedIn ? 300.f : 800.f;
		CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, TargetZoom, DeltaTime, 5.f);
	}
	
	
}

void AKHACharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Interact",IE_Pressed, this, &AKHACharacter::TryInteract);
	PlayerInputComponent->BindAction("SetDestination", IE_Pressed, this, &AKHACharacter::HandleClickLocation);
	PlayerInputComponent->BindAction("ZoomOnClick", IE_Pressed, this, &AKHACharacter::TryZoomOnClick);
	PlayerInputComponent->BindAction("ChangeLevel", IE_Pressed, this, &AKHACharacter::TryLevelChange); //레벨전환
	
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

void AKHACharacter::HandleClickLocation()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	FHitResult Hit;
	if (PC->GetHitResultUnderCursor(ECC_Visibility, true, Hit))
	{
		LastClickLocation = Hit.Location;
		bShouldMove = true;

		UE_LOG(LogTemp, Warning, TEXT("클릭된 위치 기록됨: %s"), *LastClickLocation.ToString());
	}
}



void AKHACharacter::TryZoomOnClick()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	FHitResult Hit;
	if (PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit))
	{
		if (Hit.GetActor() && Hit.GetActor()->IsA(ANPC2::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("NPC2 클릭됨 → 줌인 시작"));
			bZoomedIn = true;
		}
	}
}


void AKHACharacter::TryLevelChange()
{
	TArray<AActor*> FoundMonsters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterCharacter::StaticClass(), FoundMonsters);

	for (AActor* MonsterActor : FoundMonsters)
	{
		AMonsterCharacter* Monster = Cast<AMonsterCharacter>(MonsterActor);
		if (Monster && Monster->bPlayerIsOverlapping)
		{
			UE_LOG(LogTemp, Warning, TEXT("레벨 전환 시작!"));
			UGameplayStatics::OpenLevel(this, FName("TestMap"));
			return;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("레벨 전환 실패: 몬스터 근처 아님"));
}

