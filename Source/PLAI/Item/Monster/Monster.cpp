// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"

#include "Components/ProgressBar.h"
#include "Components/SphereComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"

#include "MonUi/MonUi.h"


// Sets default values
AMonster::AMonster()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	
	MonUiComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("MonUiComponent"));
	MonUiComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMonster::BeginPlay()
{
	Super::BeginPlay();
	
	MonsterParent = MonsterFactory->GetDefaultObject<AMonster>();
	MonUi = CreateWidget<UMonUi>(GetWorld(),MonsterParent->MonUiFactory);
	MonUiComp->SetWidget(MonUi);
	MonUiComp->SetDrawSize(FVector2D(300.f, 45.f));
	MonUiComp->SetVisibility(true);
	MonUiComp->SetRelativeLocation(FVector(0,0,150));
	MonUiComp->SetWidgetSpace(EWidgetSpace::Screen); // or World

	SetMonsterUi();
	SetHpBar();
}

// Called every frame
void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void AMonster::SetMonsterUi()
{
	MonUi->Name->SetText(FText::FromString(MonsterStruct.Name));
	MonUi->CurrentHp->SetText(FText::AsNumber(MonsterStruct.CurrentHp));
	MonUi->MaxHp->SetText(FText::AsNumber(MonsterStruct.MaxHp));
}

void AMonster::SetHpBar()
{
	MonUi->HpBar->SetPercent(static_cast<float>(MonsterStruct.CurrentHp) / MonsterStruct.MaxHp);
	MonUi->CurrentHp->SetText(FText::AsNumber(MonsterStruct.CurrentHp));
	if (MonsterStruct.CurrentHp < 1)
	{
		Destroy();
	}
}


