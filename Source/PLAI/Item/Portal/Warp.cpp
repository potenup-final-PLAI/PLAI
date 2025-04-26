// Fill out your copyright notice in the Description page of Project Settings.


#include "Warp.h"

#include "Components/SphereComponent.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"


// Sets default values
AWarp::AWarp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>("BoxComponent");
}

// Called when the game starts or when spawned
void AWarp::BeginPlay()
{
	Super::BeginPlay();

	SphereComp->OnComponentBeginOverlap.AddDynamic(this,&AWarp::OnOverlappedWarp);
}

// Called every frame
void AWarp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWarp::OnOverlappedWarp(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Display, TEXT("Wrap OnOverlappedWarp 레벨이동 들가자"));
	if (OtherActor)
	{
		if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(OtherActor))
		{
			UE_LOG(LogTemp, Display, TEXT("Wrap OnOverlappedWarp 레벨이동 들가자"));
		}
	}
}


