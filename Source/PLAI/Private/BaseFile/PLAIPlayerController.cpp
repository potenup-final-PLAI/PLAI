// Copyright Epic Games, Inc. All Rights Reserved.

#include "PLAI/Public/BaseFile/PLAIPlayerController.h"

#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "PLAI/Public/BaseFile/PLAICharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/Monster/Monster.h"
#include "PLAI/Item/Portal/Warp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/TestPlayer/InputComp/InputComp.h"
#include "PLAI/Item/UI/Inventory/Map/UiWorldMap.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

APLAIPlayerController::APLAIPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	
}

void APLAIPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// 시작 시 모든 BP_OutlineActor의 Overlay를 제거
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor->GetName().Contains(TEXT("BP_OutlineActor")))
		{
			if (UStaticMeshComponent* MeshComp = Actor->FindComponentByClass<UStaticMeshComponent>())
			{
				// 머티리얼은 기본값으로 유지, Overlay만 초기화
				MeshComp->SetOverlayMaterial(nullptr);
				//MeshComp->SetRenderCustomDepth(false); // 필요시
			}
		}
	}
	
}

void APLAIPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, true, Hit);
	AActor* HitActor = Hit.GetActor();

	if (HitActor && HitActor->GetName().Contains(TEXT("BP_OutlineActor")))
	{
		if (HitActor != PreviousOutlineActor)
		{
			// 이전 액터 Overlay 제거
			if (PreviousOutlineActor)
			{
				if (UStaticMeshComponent* PrevComp = PreviousOutlineActor->FindComponentByClass<UStaticMeshComponent>())
				{
					PrevComp->SetOverlayMaterial(nullptr);
					UE_LOG(LogTemp, Log, TEXT("❌ Overlay 제거: %s"), *PreviousOutlineActor->GetName());
				}
			}

			// 현재 감지된 액터 Overlay 설정
			if (UStaticMeshComponent* NewComp = HitActor->FindComponentByClass<UStaticMeshComponent>())
			{
				NewComp->SetOverlayMaterial(OverlayOutlineMaterial);
				UE_LOG(LogTemp, Log, TEXT("✅ Overlay 적용: %s"), *HitActor->GetName());
			}

			PreviousOutlineActor = HitActor;
		}
	}
	else
	{
		if (PreviousOutlineActor)
		{
			if (UStaticMeshComponent* MeshComp = PreviousOutlineActor->FindComponentByClass<UStaticMeshComponent>())
			{
				MeshComp->SetOverlayMaterial(nullptr);
				UE_LOG(LogTemp, Log, TEXT("❌ 마우스 벗어남, Overlay 제거: %s"), *PreviousOutlineActor->GetName());
			}

			PreviousOutlineActor = nullptr;
		}
	}
	
	
}

void APLAIPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext,1);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &APLAIPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &APLAIPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &APLAIPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &APLAIPlayerController::OnSetDestinationReleased);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &APLAIPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &APLAIPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &APLAIPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &APLAIPlayerController::OnTouchReleased);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APLAIPlayerController::OnInputStarted()
{
	StopMovement();
	if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		TestPlayer->InputComp->SetMappingContext();
		TestPlayer->InputComp->BindInputActions();
	}
}

// Triggered every frame when the input is held down
void APLAIPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();

	// UE_LOG(LogTemp,Warning,TEXT("PLAIPLayerController 오른쪽 마우스 트리거중 %f"), FollowTime);
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location; //위치 저장
	}
	
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void APLAIPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void APLAIPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void APLAIPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void APLAIPlayerController::HandleMouseOutline()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, true, Hit);

	AActor* HitActor = Hit.GetActor();

	if (HitActor != PreviousOutlineActor)
	{
		// 이전 액터 외곽선 끄기
		if (PreviousOutlineActor)
		{
			UPrimitiveComponent* PrevComp = Cast<UPrimitiveComponent>(PreviousOutlineActor->GetComponentByClass(UPrimitiveComponent::StaticClass()));
			if (PrevComp)
			{
				PrevComp->SetRenderCustomDepth(false);
			}
		}

		// 새로운 액터 외곽선 켜기
		if (HitActor)
		{
			UPrimitiveComponent* NewComp = Cast<UPrimitiveComponent>(HitActor->GetComponentByClass(UPrimitiveComponent::StaticClass()));
			if (NewComp)
			{
				NewComp->SetRenderCustomDepth(true);
			}
		}

		PreviousOutlineActor = HitActor;
	}
}

void APLAIPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APLAIPlayerController, TestPlayers);
}



void APLAIPlayerController::Server_WarpPlayer_Implementation(EMonSpawnType SpawnType)
{
	TArray<AActor*> Mons;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonster::StaticClass(), Mons);

	for (AActor* Mon : Mons)
	{
		if (AMonster* Monster = Cast<AMonster>(Mon))
		{
			UE_LOG(LogTemp,Warning,TEXT("PLAIPlayerController 몬스터 삭제"));
			Monster->Destroy();
			break;
		}
	}
	
	UE_LOG(LogTemp,Warning,TEXT("UiPortal 어디소환중? [%s]"),*UEnum::GetValueAsString(SpawnType))
	
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWarp::StaticClass(), Actors);
	
	if (Actors.Num() == 0){UE_LOG(LogTemp,Warning,TEXT("UiPortal 없냐? %d"),Actors.Num())return;}
	
	for (AActor* Actor : Actors)
	{
		if (AWarp* MonSpawn = Cast<AWarp>(Actor))
		{
			if (MonSpawn->MonSpawnType ==  SpawnType)
			{
				GetPawn()->SetActorLocation(MonSpawn->GetActorLocation() + FVector(250,0,1000));
			}
		}
	}
}
