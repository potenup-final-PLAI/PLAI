// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterMaster.h"

#include "Components/WidgetComponent.h"
#include "MonUi/MonUi.h"


// Sets default values
AMonsterMaster::AMonsterMaster()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMonsterMaster::BeginPlay()
{
	Super::BeginPlay();
	
	MonsterMaster = MonsterMasterFactory->GetDefaultObject<AMonster>();
	if (MonsterMaster)
	{
		UE_LOG(LogTemp, Warning, TEXT("Monster Master Created [%s]"),*MonsterMaster->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Monster Master 없음 Created"));
	}
	if (MonsterMaster->MonUiFactory)
	{
		UE_LOG(LogTemp, Warning, TEXT("Monster Master Factory Created [%s]"),*MonsterMaster->MonUi->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Monster Master Factory 없음"));
	}
	MonUi = CreateWidget<UMonUi>(GetWorld(),MonsterMaster->MonUiFactory);
	MonUiComp->SetWidget(MonUi);
	MonUiComp->SetDrawSize(FVector2D(300.f, 45.f));
	MonUiComp->SetVisibility(true);
	MonUiComp->SetRelativeLocation(FVector(0,0,150));
	MonUiComp->SetWidgetSpace(EWidgetSpace::Screen); // or World
}

// Called every frame
void AMonsterMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMonsterMaster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

