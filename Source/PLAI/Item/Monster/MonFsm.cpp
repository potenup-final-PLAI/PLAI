// Fill out your copyright notice in the Description page of Project Settings.


#include "MonFsm.h"

#include "Monster.h"


// Sets default values for this component's properties
UMonFsm::UMonFsm()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMonFsm::BeginPlay()
{
	Super::BeginPlay();
	Monster = Cast<AMonster>(GetOwner());
	
	LineDestination();
}


// Called every frame
void UMonFsm::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (MonState)
	{
	case EMonState::Idle:
		Idle();
		break;
	case EMonState::Around:
		Around();
		break;
	case EMonState::Attack:
		Attack();
		break;
	}

	// ...
}

void UMonFsm::Idle()
{
	// MoveDestination();
	
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Monster);
	FVector Start = Monster->GetActorLocation() + RandLocation() + FVector(0,0,2000);
	FVector End = Start + FVector(0,0,10000);
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Params);
	if(bHit)
	{
		TargetLocation = Hit.ImpactPoint;
		DrawDebugLine(GetWorld(),Start,TargetLocation,FColor::Blue,false,0.02f);
		DrawDebugSphere(GetWorld(),TargetLocation,10,10,FColor::Blue,false,2.0f);
	}
	
	// CurrentTime += GetWorld()->GetDeltaSeconds();
	//
	// FVector distVec = TargetLocation - Monster->GetActorLocation();
	// distLength = distVec.Length();
	// Monster->AddActorWorldOffset(distVec.GetSafeNormal() * 10);
	//
	// if (CurrentTime > 2)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("2초 타이머 거리는 %f"), distLength);
	// 	
	// 	if (distLength < 100.f)
	// 	{
	// 		TargetLocation = RandLocation();
	// 	}
	// 	CurrentTime = 0.0f;
	// }
}

void UMonFsm::Around()
{
	
}

void UMonFsm::Attack()
{
}

FVector UMonFsm::RandLocation(float X, float Y, float Z)
{
	float x = FMath::RandRange(-X, X);
	float y = FMath::RandRange(-Y, Y);
	float z = FMath::RandRange(0.0f, Z);

	FVector RandLoc = FVector(x, y, z);

	return FVector(RandLoc);
}

void UMonFsm::MoveDestination()
{
	MyTimer([this]
	{
		DrawDebugLine(GetWorld(),Monster->GetActorLocation(),TargetLocation,FColor::Blue,false,0.02f);
		FVector Distance = TargetLocation - Monster->GetActorLocation();
		Monster->AddActorWorldOffset(Distance.GetSafeNormal() * 15);
		
		if (Timer == true && Distance.Length() < 100)
		{
			UE_LOG(LogTemp,Warning,TEXT("MonFsm 로케이션 타이머 어디까지왔니 %f"),Distance.Length())
			LineDestination();
		}
	},2.0f);
}

void UMonFsm::LineDestination()
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Monster);

	FVector Start = Monster->GetActorLocation() + RandLocation() + FVector(0,0,2000);
	FVector End = Start + FVector(0,0,10000);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Params);
	if(bHit)
	{
		TargetLocation = Hit.ImpactPoint;
		DrawDebugLine(GetWorld(),Start,TargetLocation,FColor::Blue,false,0.02f);
		DrawDebugSphere(GetWorld(),TargetLocation,10,10,FColor::Blue,false,2.0f);
	}
}


void UMonFsm::MyTimer(void(UMonFsm::* Func)(), float time)
{
	Timer = false;
	
	CurrentTime += GetWorld()->GetDeltaSeconds();
	
	if (CurrentTime > time)
	{
		(this->*Func)();
		Timer = true;
	}
	CurrentTime = 0.0f;
}

void UMonFsm::MyTimer(TFunction<void()> Func, float time)
{
	Timer = false;
	
	CurrentTime += GetWorld()->GetDeltaSeconds();
	
	if (CurrentTime > time)
	{
		Func();
		Timer = true;
	}
	CurrentTime = 0.0f;
}


