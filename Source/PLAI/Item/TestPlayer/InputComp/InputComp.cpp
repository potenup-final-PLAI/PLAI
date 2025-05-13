// Fill out your copyright notice in the Description page of Project Settings.


#include "InputComp.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AI/NavigationSystemBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"


// Sets default values for this component's properties
UInputComp::UInputComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInputComp::BeginPlay()
{
	Super::BeginPlay();

	TestPlayer = Cast<ATestPlayer>(GetOwner());
	Pc = Cast<APlayerController>(TestPlayer->GetController());
	
	if (ULocalPlayer* LP = Pc->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
	if (UEnhancedInputComponent* InputComp = Cast<UEnhancedInputComponent>(TestPlayer->InputComponent))
	{
		InputComp->BindAction(IE_Equip, ETriggerEvent::Started, this, &UInputComp::On_Equip);
		InputComp->BindAction(IE_Inven, ETriggerEvent::Started, this, &UInputComp::On_Inven);
		InputComp->BindAction(IE_LeftMouse, ETriggerEvent::Started, this, &UInputComp::On_LeftMouseStart);
		InputComp->BindAction(IE_LeftMouse, ETriggerEvent::Triggered, this, &UInputComp::On_LeftMouseTriggered);
	}
}
// ...

// Called every frame
void UInputComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInputComp::On_Equip()
{
	UE_LOG(LogTemp, Warning, TEXT("InputComp On Equip"));
}

void UInputComp::On_Inven()
{
	UE_LOG(LogTemp, Warning, TEXT("InputComp On Inven"));
}

void UInputComp::On_LeftMouseStart()
{
	UE_LOG(LogTemp, Warning, TEXT("InputComp On LeftMouseStart"));
	TestPlayer->GetController()->StopMovement();
	TimeCamera = 0;
}

void UInputComp::On_LeftMouseTriggered()
{
	if (!Pc->IsLocalController()) return;

	WalkTime += GetWorld()->GetDeltaSeconds();
	FHitResult Hit;
	Pc->GetHitResultUnderCursor(ECC_Visibility, true, Hit);
	TestPlayer->AddMovementInput((Hit.Location - TestPlayer->GetActorLocation()).GetSafeNormal(),1.0f,false);

	TimeCamera += GetWorld()->GetDeltaSeconds();

	float Dot = FVector::DotProduct(FVector(TestPlayer->GetActorForwardVector().X,TestPlayer->GetActorForwardVector().Y,0),
		FVector((Hit.Location-TestPlayer->GetActorLocation()).GetSafeNormal().X,(Hit.Location-TestPlayer->GetActorLocation().GetSafeNormal()).Y,0));
	UE_LOG(LogTemp, Warning, TEXT("InputComp On LeftMouseTriggered 플레이어 카메라 전방 내적값은? [%f]"),Dot);

	// TestPlayer->CameraBoom->AddWorldRotation(FRotator());	// if (TimeCamera > 3)
	// {TimeCamera = 3; }
}

