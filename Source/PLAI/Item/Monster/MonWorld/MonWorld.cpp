// Fill out your copyright notice in the Description page of Project Settings.


#include "MonWorld.h"

#include "MonWorldFsm.h"
#include "Engine/OverlapResult.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"


// Sets default values
AMonWorld::AMonWorld()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMonWorld::BeginPlay()
{
	Super::BeginPlay();
	InitLoc = GetActorLocation() + RandLocation();
	FirstInitLoc = GetActorLocation();

	if (MonsterType == EMonsterType::Boss)
	{
		
	}
}

// Called every frame
void AMonWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority()){return;}
	
	CurrentTimeTwo += DeltaTime;
	
	if (MonsterType == EMonsterType::Monster)
	{ MoveToLocation(); }
	
	if (CurrentTimeTwo > 1.0)
	{
		if (MonsterType == EMonsterType::Boss)
		{ CastPlayer(); }
		CurrentTimeTwo = 0;
	}
}

// Called to bind functionality to input
void AMonWorld::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

FVector AMonWorld::RandLocation()
{
	float x = FMath::FRandRange(-RandLocationDist,RandLocationDist);
	float y = FMath::FRandRange(-RandLocationDist,RandLocationDist);
	float z = FMath::FRandRange(20.f,100.f);

	FHitResult hit;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	FCollisionObjectQueryParams objParams;
	objParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);

	bool bHit = GetWorld()->LineTraceSingleByObjectType
	(hit,FVector(x,y,z) + FVector(0,0,2000),
		FVector(x,y,z) + FVector(0,0,-3000),objParams, params);
	
	// DrawDebugLine(GetWorld(),GetActorLocation(),hit.Location,FColor::Blue,
	// 	false,2);
	// DrawDebugSphere(GetWorld(),hit.Location,30,30,FColor::Red,false,2);
	return FVector(hit.Location);
}

void AMonWorld::MoveToLocation()
{
	CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime > 1)
	{
		CastObject();
		CastPlayer();
		CurrentTime = 0;
	}
	if (FVector::Distance(FVector(GetActorLocation().X,GetActorLocation().Y,0),FVector(InitLoc.X,InitLoc.Y,0)) > 25)
	{
		if (bBattle == false)
		{
			FVector Dist = InitLoc - GetActorLocation(); 
			AddActorWorldOffset(Dist.GetSafeNormal() * 5);
		
		}
	}
	else
	{
		FVector Candidate;
		do
		{ Candidate = GetActorLocation() + RandLocation(); }
		while (FVector::Distance(Candidate, FirstInitLoc) >  InitLocationDist);
		
		InitLoc = Candidate;
		FVector Dist = InitLoc - GetActorLocation(); 
		SetActorRotation(Dist.Rotation());
	}
}

TArray<FOverlapResult> AMonWorld::GetHitResult(float Distance)
{
	if (UIMonWorld){ UIMonWorld->RemoveFromParent(); }bBattle = false;
	TArray<FOverlapResult> Hits; FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	bool bHit = GetWorld()->OverlapMultiByChannel(Hits,GetActorLocation(),
		FQuat::Identity,ECC_Pawn,FCollisionShape::MakeSphere(Distance),Params);
	if (bHit){ return Hits; }

	// DrawDebugSphere(GetWorld(),GetActorLocation(),Distance,10,FColor::Blue,false,1);

	return TArray<FOverlapResult>();
}

void AMonWorld::CastPlayer()
{
	if (UIMonWorld)
	{ UIMonWorld->RemoveFromParent(); }
	bBattle = false;

	// UE_LOG(LogTemp,Error,TEXT("AMonWorld::CastPlayer 캐스팅 플레이어중"));
	TArray<FOverlapResult> Hits;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->OverlapMultiByChannel(Hits,GetActorLocation(),FQuat::Identity,ECC_Pawn,
		FCollisionShape::MakeSphere(CastingDistance),Params);
	// DrawDebugSphere(GetWorld(),GetActorLocation(),CastingDistance,10,FColor::Blue,false,1);
	if (bHit)
	{
		for (FOverlapResult Hit : Hits)
		{
			if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(Hit.GetActor()))
			{
				FVector Dir = TestPlayer->GetActorLocation() - GetActorLocation();
				//if (FVector::DotProduct(GetActorForwardVector() ,Dir.GetSafeNormal()) > 0)
				{
					UIMonWorld = CreateWidget<UUiMonWorld>(GetWorld(),UIMonWorldFactory);
					UIMonWorld->AddToViewport();
					
					UIMonWorld->MonWorld = this;
					UIMonWorld->TestPlayer = TestPlayer;
					bBattle = true;
					break;
				}
			}
		}
	}
}

void AMonWorld::CastObject()
{
	TArray<FHitResult> Hits;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->SweepMultiByChannel(Hits,GetActorLocation(),GetActorLocation() + GetActorForwardVector() * 25,
		FQuat::Identity,ECC_Visibility,FCollisionShape::MakeBox(FVector(25,25,25)),Params);
	
	if (bHit)
	{
		UE_LOG(LogTemp,Warning,TEXT("AMonWorld::CastObject 박스 콜리전 부딛힘"));
		for (FHitResult Hit : Hits)
		{
			if (FVector::Distance(Hit.Location,GetActorLocation()) < 50)
			{
				InitLoc = GetActorLocation() + RandLocation();
				break;
			}
		}
	}
}

// 랜드 Loc 구하는 식 디버그

// InitLoc = GetActorLocation() + RandLocation();
// FVector Dist = InitLoc - GetActorLocation(); 
// SetActorRotation(Dist.Rotation());

// UE_LOG(LogTemp,Warning,TEXT("MonWorld 25보다 거리 적음 다음 이동 근데 거리는?[%f]"),FVector::Distance(GetActorLocation(),InitLoc));
// DrawDebugSphere(GetWorld(),InitLoc,50,20,FColor::Blue,false,2);

// UE_LOG(LogTemp,Warning,TEXT("MonWorld 25보다 거리 적음 다음 이동 큼 거리는?[%f]"),FVector::Distance(GetActorLocation(),InitLoc));
// DrawDebugSphere(GetWorld(),InitLoc,50,20,FColor::Red,false);