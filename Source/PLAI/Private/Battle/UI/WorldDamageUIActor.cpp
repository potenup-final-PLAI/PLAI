// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/UI/WorldDamageUIActor.h"

#include "Battle/UI/WorldDamageUI.h"
#include "Components/WidgetComponent.h"


// Sets default values
AWorldDamageUIActor::AWorldDamageUIActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	damageUIComp = CreateDefaultSubobject<UWidgetComponent>("damageUIComp");
	SetRootComponent(damageUIComp);
	damageUIComp->SetGenerateOverlapEvents(false); // 충돌 막기
	damageUIComp->SetVisibility(false); 
	damageUIComp->SetCastShadow(false);
}

// Called when the game starts or when spawned
void AWorldDamageUIActor::BeginPlay()
{
	Super::BeginPlay();

	damageUI = Cast<UWorldDamageUI>(damageUIComp->GetUserWidgetObject());
	if (!damageUI)
	{
		UE_LOG(LogTemp, Error, TEXT("WorldDamageUIActor: damageUI is null!"));
		return;
	}
	GetWorld()->GetTimerManager().SetTimer(damageTimerHandle, this, &AWorldDamageUIActor::MoveUI, 0.1f, true);
}

// Called every frame
void AWorldDamageUIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AWorldDamageUIActor::MoveUI()
{
	if (bShowUI && index <= 60)
	{
		ShowDamageUI();
		FVector P0 = GetActorLocation();
		FVector vt = GetActorUpVector() * speed * GetWorld()->GetDeltaSeconds();
		FVector P = P0 + vt;
		SetActorLocation(P);
		++index;
	}
	else
	{
		index = 0;
		HideDamageUI();
	}
}

void AWorldDamageUIActor::ShowDamageUI()
{
	bShowUI = true;
	damageUIComp->SetVisibility(true);
}

void AWorldDamageUIActor::HideDamageUI()
{
	bShowUI = false;
	damageUIComp->SetVisibility(false);
}
