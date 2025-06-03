// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcCharacter.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Camera/CameraComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "UObject/UObjectGlobals.h" 
#include "PLAI/Item/TestPlayer/TestPlayer.h"


// Sets default values
ANpcCharacter::ANpcCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	WidgetComp = CreateDefaultSubobject<UWidgetComponent>("Widget");
	WidgetComp->SetupAttachment(RootComponent);
	WidgetComp->SetMobility(EComponentMobility::Movable);
	
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>("Niagara");
	NiagaraComp->SetupAttachment(RootComponent);

	QuestActorComponent = CreateDefaultSubobject<UChildActorComponent>("QuestActor");
	QuestActorComponent->SetupAttachment(GetRootComponent());
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
	NiagaraComp->SetRelativeLocation(FVector(0,0,120));

	WidgetComp->SetWidgetSpace(EWidgetSpace::World);

	// WidgetComp->SetRelativeRotation(FRotator(-90.0f,90.0f,0.0f));
	
	TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if (TestPlayer)
	{ UE_LOG(LogTemp,Display,TEXT("ANpcCharacter::TestPlayer [%s]"),*TestPlayer->GetName()); }
	else
	{ UE_LOG(LogTemp,Display,TEXT("ANpcCharacter::TestPlayer 없음")); }
	
	if (UNiagaraSystem* LoadedSystem = LoadObject<UNiagaraSystem>(nullptr,TEXT("/Game/sA_PickupSet_1/Fx/NiagaraSystems/NS_Magic_1.NS_Magic_1")))
	{ NiagaraComp->SetAsset(LoadedSystem); }
	else
	{ UE_LOG(LogTemp, Warning, TEXT("Niagara System 로드 실패! 경로 확인 필요")); }

	if (!UiNpc) return;
	UiNpc->NpcName->SetText(FText::FromString(NpcNameString));
	// TWeakObjectPtr <UUiNpc> WeakUiNpc = UiNpc;
	// FString NpcName = NpcNameString;  
	// FTimerHandle TimerHandle;
	// GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this,NpcName,WeakUiNpc]()
	// {
	// 	if (!IsValid(UiNpc)) return;
	// 	WeakUiNpc->NpcName->SetText(FText::FromString(NpcName));},2.0,false);
}

// Called every frame
void ANpcCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!TestPlayer) return;
	FVector mCameraVec = -TestPlayer->TopDownCameraComponent->GetForwardVector(); 
	WidgetComp->SetWorldRotation(mCameraVec.Rotation());
}

// Called to bind functionality to input
void ANpcCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
