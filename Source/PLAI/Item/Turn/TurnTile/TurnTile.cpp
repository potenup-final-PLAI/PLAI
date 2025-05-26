// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnTile.h"

#include "Misc/MapErrors.h"


// Sets default values
ATurnTile::ATurnTile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	SetRootComponent(StaticMeshComp);
}

// Called when the game starts or when spawned
void ATurnTile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATurnTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime > 2)
	{
		UE_LOG(LogTemp,Warning,TEXT("ATurnTile:: 2초뒤 로케이션 %s"),*GetActorLocation().ToString());
		DrawDebugSphere(GetWorld(), GetActorLocation() + FVector(0,0,50), 100, 20, FColor::Red,false,2,2);
		CurrentTime = 0;
	}
}

void ATurnTile::GetLocalBox()
{
	const FBox LocalBox = StaticMeshComp->GetStaticMesh()->GetBoundingBox();
	const FVector LocalEndPoint = LocalBox.Max;

	const FTransform& CompTransform = StaticMeshComp->GetComponentTransform();
	const FVector WorldEndPoint = CompTransform.TransformPosition(LocalEndPoint);
}

