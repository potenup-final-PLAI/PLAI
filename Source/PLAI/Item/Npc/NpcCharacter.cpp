// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcCharacter.h"

#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"


// Sets default values
ANpcCharacter::ANpcCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	WidgetComp = CreateDefaultSubobject<UWidgetComponent>("Widget");
	WidgetComp->SetupAttachment(RootComponent);
	// WidgetComp->SetMobility(EComponentMobility::Movable);



	

	
	
}

// Called when the game starts or when spawned
void ANpcCharacter::BeginPlay()
{
	Super::BeginPlay();
	UiNpc = CreateWidget<UUiNpc>(GetWorld(),UiNpcFactory);
    WidgetComp->SetWidget(UiNpc);
	WidgetComp->SetDrawSize(FVector2D(300.0f,45.0f));
	WidgetComp->SetVisibility(true);
	WidgetComp->SetRelativeLocation(FVector(0,0,150));
	WidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
	{ UiNpc->NpcName->SetText(FText::FromString(NpcNameString));},0.5,false);
}

// Called every frame
void ANpcCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ANpcCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

