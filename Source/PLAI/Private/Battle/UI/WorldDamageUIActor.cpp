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

	bReplicates = true;
}

// Called when the game starts or when spawned
void AWorldDamageUIActor::BeginPlay()
{
	Super::BeginPlay();

	damageUI = Cast<UWorldDamageUI>(damageUIComp->GetUserWidgetObject());
	if (!damageUI)
	{
		UE_LOG(LogTemp, Error, TEXT("WorldDamageUIActor: damageUI is null!"));
	}
}

// Called every frame
void AWorldDamageUIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (bShowUI)
	{
		elapsed += DeltaTime;
		float Alpha = FMath::Clamp(elapsed / moveDuration, 0.f, 1.f);

		// EaseOut : 처음 빠르고 나중에 느려짐
		float EasedAlpha = FMath::InterpEaseOut(0.f, 1.f, Alpha, 2.0f);

		FVector NewLoc = FMath::Lerp(startLoc, endLoc, EasedAlpha);
		SetActorLocation(NewLoc);

		if (Alpha >= 1.f)
		{
			// UI 다시 안보이게 처리
			HideDamageUI();
		}
	}
}

void AWorldDamageUIActor::ShowDamageUI()
{
	bShowUI = true;
	damageUIComp->SetVisibility(true);

	startLoc = GetActorLocation();
	endLoc = startLoc + FVector(0, 0, 60.f);
	elapsed = 0.f;
}

void AWorldDamageUIActor::HideDamageUI()
{
	bShowUI = false;
	damageUIComp->SetVisibility(false);
}
