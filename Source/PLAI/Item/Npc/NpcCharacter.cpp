// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"


// Sets default values
ANpcCharacter::ANpcCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	WidgetComp = CreateDefaultSubobject<UWidgetComponent>("Widget");
	WidgetComp->SetupAttachment(RootComponent);
	WidgetComp->SetMobility(EComponentMobility::Movable);
}

// Called when the game starts or when spawned
void ANpcCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UiNpc = CreateWidget<UUiNpc>(GetWorld(),UiNpcFactory);
    WidgetComp->SetWidget(UiNpc);
	WidgetComp->SetVisibility(true);
	WidgetComp->SetRelativeLocation(FVector(0,0,150));
	WidgetComp->SetDrawSize(FVector2D(300.0f,120.0f));
	WidgetComp->SetWidgetSpace(EWidgetSpace::World);

	// WidgetComp->SetRelativeRotation(FRotator(-90.0f,90.0f,0.0f));
	
	TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if (TestPlayer)
	{ UE_LOG(LogTemp,Display,TEXT("ANpcCharacter::TestPlayer [%s]"),*TestPlayer->GetName()); }
	else
	{ UE_LOG(LogTemp,Display,TEXT("ANpcCharacter::TestPlayer 없음")); }
	
	
	// FTimerHandle TimerHandle;
	// GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
	// { UiNpc->NpcName->SetText(FText::FromString(NpcNameString));},0.5,false);
}

// Called every frame
void ANpcCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector mCameraVec = -TestPlayer->TopDownCameraComponent->GetForwardVector(); 
	WidgetComp->SetWorldRotation(mCameraVec.Rotation());
}

// Called to bind functionality to input
void ANpcCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
