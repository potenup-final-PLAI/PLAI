// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/Tile/GridTile.h"

#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Net/UnrealNetwork.h"
// Sets default values
AGridTile::AGridTile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(boxComp);
	boxComp->SetRelativeScale3D(FVector(1.0f, 1.0f, 0.1f));
	boxComp->SetBoxExtent(FVector(50));

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));

	// 변수 초기화
	sCostValue = 0;
	tCostValue = 0;
	parentTile = nullptr;
	gridCoord = FIntPoint(0, 0);
	bReplicates = true;
}

// Called when the game starts or when spawned
void AGridTile::BeginPlay()
{
	Super::BeginPlay();
	
	InitDecal();
}

void AGridTile::GetLifetimeReplicatedProps(
	TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AGridTile, gridCoord);
	DOREPLIFETIME(AGridTile, sCostValue);
	DOREPLIFETIME(AGridTile, tCostValue);
	DOREPLIFETIME(AGridTile, parentTile);
}

// Called every frame
void AGridTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGridTile::InitDecal()
{
	dynDecalInstance = meshComp->CreateDynamicMaterialInstance(0);
	if (dynDecalInstance)
	{
		if (dynDecalInstance)
		{
			dynDecalInstance->SetScalarParameterValue(TEXT("TileOpacity"), 0.0f);
		}
	}
}

void AGridTile::SetGridCoord(FIntPoint coord)
{
	gridCoord = coord;
	// UE_LOG(LogTemp, Warning, TEXT("Coord X = %d Coord Y = %d, TileName = %s"), gridCoord.X, gridCoord.Y, *this->GetActorNameOrLabel());
}

void AGridTile::SetCost(AGridTile* s, AGridTile* g)
{
	if (!IsValid(s) || !IsValid(g))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetCost : Invalid pointers passed"));
		return;
	}
	// 이전 타일의 위치
	FVector prevPos = s->GetActorLocation();
	// 목적지 타일의 위치
	FVector goalPos = g->GetActorLocation();
	// 현재 나의 위치
	FVector curPos = GetActorLocation();

	// sCost 구하기
	sCostValue = FMath::Abs(prevPos.X - curPos.X) + FMath::Abs(
		prevPos.Y - curPos.Y);
	sCostValue += s->sCostValue;

	// gCost 구하기
	float gCostValue = FMath::Abs(goalPos.X - curPos.X) + FMath::Abs(
		goalPos.Y - curPos.Y);

	// tCost 구하기
	tCostValue = sCostValue + gCostValue;

	// 누구를 기준으로 Cost를 계산했냐?
	parentTile = s;
}
