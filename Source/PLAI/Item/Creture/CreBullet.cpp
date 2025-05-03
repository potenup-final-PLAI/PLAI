// Fill out your copyright notice in the Description page of Project Settings.


#include "CreBullet.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
ACreBullet::ACreBullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(RootComponent);
	
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AudioComponent"));
	NiagaraComp->SetupAttachment(RootComponent);
	
	ProjectileComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile"));
	ProjectileComp->InitialSpeed = 2000.0f;
	ProjectileComp->MaxSpeed = 4000.0f;
	
	ProjectileComp->bRotationFollowsVelocity = true;
	ProjectileComp->bShouldBounce = false;
}

// Called when the game starts or when spawned
void ACreBullet::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACreBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

