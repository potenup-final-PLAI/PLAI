// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnTile.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Misc/MapErrors.h"
#include "PLAI/Item/Monster/MonsterMaster.h"
#include "PLAI/Item/Monster/MonsterStruct.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/TestPlayer/TurnPlayer.h"
#include "PLAI/Item/Turn/TurnMoster/TurnMonster.h"


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

	MonsterSpawn();
	PlayerSpawn();
}

// Called every frame
void ATurnTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime > 2)
	{
		// GetTileCorner();
		// MonsterSpawn();
		// PlayerSpawn();
		CurrentTime = 0;
	}
}


TArray<FVector> ATurnTile::GetTileCorner()
{
	FVector LocalOrigin, LocalExtend;
	StaticMeshComp->GetLocalBounds(LocalOrigin, LocalExtend);
	FVector Scale = StaticMeshComp->GetComponentScale();
	
	FVector ScaledMin = GetActorLocation() + Scale * LocalOrigin + FVector(0,0,Scale.Z*LocalExtend.Z - Scale.Z*LocalOrigin.Z);
	FVector ScaledOrigin = GetActorLocation()                    + FVector(0,0,Scale.Z*LocalExtend.Z - Scale.Z*LocalOrigin.Z)/2;
	FVector ScaledExtend = GetActorLocation() + Scale * LocalExtend;

	TArray<FVector> Corners;
	
	Corners.Add(ScaledMin);
	Corners.Add(ScaledOrigin);
	Corners.Add(ScaledExtend);

	return Corners;
}

void ATurnTile::MonsterSpawn()
{
	for (int i = 0; i < 2; i++)
	{
		float x = FMath::FRandRange(GetTileCorner()[1].X, GetTileCorner()[2].X);
		float y = FMath::FRandRange(GetTileCorner()[0].Y, GetTileCorner()[2].Y);
		float z = GetTileCorner()[0].Z;

		MonsterSpawnTable(FVector(x,y,z));
		DrawDebugSphere(GetWorld(),FVector(x,y,z), 100, 20, FColor::Red,false,2,2);
	}
}

void ATurnTile::MonsterSpawnTable(FVector SpawnLocation)
{
	TArray<FName>RowNames = MonsterTable->GetRowNames();
	FTurnMonsterStruct* TurnMonsterStruct = MonsterTable->FindRow<FTurnMonsterStruct>(RowNames[0],TEXT("TurnTile"));
	if (ATurnMonster* TurnMonster = GetWorld()->SpawnActor<ATurnMonster>(TurnMonsterStruct->TurnMonster))
	{
		TurnMonster->SetActorLocation(SpawnLocation);
		TurnMonster->TurnMonsterStruct = *TurnMonsterStruct;
		TurnMonster->SetMonsterUi();
	}

	for (FName RowName : RowNames)
	{ }
}

void ATurnTile::PlayerSpawn()
{
	for (int i = 0; i < 2; i++)
	{
		float x = FMath::FRandRange(GetTileCorner()[0].X, GetTileCorner()[1].X);
		float y = FMath::FRandRange(GetTileCorner()[0].Y, GetTileCorner()[2].Y);
		float z = GetTileCorner()[0].Z + 185;

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		if (ATurnPlayer* TurnPlayer = GetWorld()->SpawnActor<ATurnPlayer>(TurnPlayerFactory,Params))
		{
			TurnPlayer->TileRange2D = FVector2D(GetTileCorner()[0].X-GetTileCorner()[2].X, GetTileCorner()[0].Y-GetTileCorner()[2].Y);
			{UE_LOG(LogTemp,Error,TEXT("TurnTile Player Spawn TileRange2D크기 [%s]"),*TurnPlayer->TileRange2D.ToString());}
			TurnPlayer->SetActorLocation(FVector(x,y,z));
			TurnPlayer->SetActorScale3D(FVector(2.0,2.0,2.0));
		}
		else{UE_LOG(LogTemp,Error,TEXT("TurnTile Player Spawn Failed"));}
		DrawDebugSphere(GetWorld(),FVector(x,y,z), 100, 20, FColor::Blue,false,2,2);
	}
}


// DrawDebugSphere(GetWorld(),ScaledMin,    100, 20, FColor::Red,false,2,2);
// DrawDebugSphere(GetWorld(),GetActorLocation(), 100, 20, FColor::Red,false,2,2);
// DrawDebugSphere(GetWorld(),ScaledOrigin, 100, 20, FColor::Red,false,2,2);