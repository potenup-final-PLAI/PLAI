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
		InputComp->BindAction(IE_LeftMouse, ETriggerEvent::Completed, this, &UInputComp::On_LeftMouseComplete);
		InputComp->BindAction(IE_MouseWheel, ETriggerEvent::Triggered, this, &UInputComp::On_MouseWheelTriggered);
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
	if (!Pc->IsLocalController()) return;

	UE_LOG(LogTemp, Warning, TEXT("InputComp On Equip"));
}

void UInputComp::On_Inven()
{
	if (!Pc->IsLocalController()) return;

	UE_LOG(LogTemp, Warning, TEXT("InputComp On Inven"));
}

void UInputComp::On_LeftMouseStart()
{
	if (!Pc->IsLocalController()) return;

	UE_LOG(LogTemp, Warning, TEXT("InputComp On LeftMouseStart true"));
	bLeftMouse = true;

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

	// 오른쪽 나의 벡터와 카메라 전방 벡터 내적
	float Dot = FVector::DotProduct(FVector(TestPlayer->GetActorRightVector().X,TestPlayer->GetActorRightVector().Y,0),
		FVector(TestPlayer->CameraBoom->GetForwardVector().X,TestPlayer->CameraBoom->GetForwardVector().Y,0));
    // 나의 전방 카메라 전방 벡터 내적

	if (Dot > 0.13)
	{ TestPlayer->CameraBoom->AddWorldRotation(FRotator(0,-1,0)); }
	else if (Dot < -0.13)
	{ TestPlayer->CameraBoom->AddWorldRotation(FRotator(0,+1,0)); }

	
	else if (FMath::Abs(Dot) > 0.6)
	{
		MouseTime += GetWorld()->GetDeltaSeconds();
		{
			if (Dot > 0.6)
			{ TestPlayer->CameraBoom->AddWorldRotation(FRotator(0,-FMath::Sin(90 * MouseTime) * 10,0)); }
			else if (Dot < -0.6)
			{ TestPlayer->CameraBoom->AddWorldRotation(FRotator(0,FMath::Sin(90 * MouseTime) * 10,0)); }
		}
		if (MouseTime > 1)
		{
			MouseTime = 1;
		}
	}
	
	// UE_LOG(LogTemp, Warning, TEXT("InputComp On LeftMouseTriggered 플레이어 카메라 전방 내적값은? [%f]"),Dot);
	float MouseX, MouseY;
	Pc->GetInputMouseDelta(MouseX, MouseY);
	FRotator CameraBoomRot = TestPlayer->CameraBoom->GetRelativeRotation();

	MousePower += MouseY;
	
	UE_LOG(LogTemp, Warning, TEXT("InputComp On LeftMouseTriggered 마우스 Y축 힘 값은? [%f]"),MousePower);

	if (FMath::Abs(MousePower) > 5.f)
	{
		CameraBoomRot.Pitch = FMath::Clamp(CameraBoomRot.Pitch + 0.5 * MouseY, - 60.0f, 10.0f);
		TestPlayer->CameraBoom->SetRelativeRotation(CameraBoomRot);
	}
}

void UInputComp::On_LeftMouseComplete()
{
	if (!Pc->IsLocalController()) return;

	bLeftMouse = false;
	MousePower = 0;
	MouseTime = 0;
	UE_LOG(LogTemp, Warning, TEXT("InputComp On LeftMouseStart Completed false"));
}

void UInputComp::On_MouseWheelTriggered(const FInputActionValue& Value)
{
	float Axis = Value.Get<float>();
	UE_LOG(LogTemp, Warning, TEXT("InputComp On LeftMouseWheel Value [%f]"),Axis);
	TestPlayer->CameraBoom->TargetArmLength -= 250 * Axis;
}

