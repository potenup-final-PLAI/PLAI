// Fill out your copyright notice in the Description page of Project Settings.


#include "MonSpawn.h"

#include "Blueprint/UserWidget.h"
#include "PLAI/Item/Monster/Monster.h"
#include "Templates/Function.h"

// Sets default values
AMonSpawn::AMonSpawn()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMonSpawn::BeginPlay()
{
	Super::BeginPlay();

	AMonster* Monster = GetWorld()->SpawnActor<AMonster>(MonsterFactory[1]);
	 	
	Monster->SetActorLocation(GetActorLocation() + FVector(100,100,0));
	
	UE_LOG(LogTemp,Warning,TEXT("MonsterSpwan 생성됨"));
}

// Called every frame
void AMonSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpawnMonster();
	// CurrentTime += DeltaTime;
	// if (CurrentTime > 1.0f)
	// {
	// 	float x = FMath::RandRange(-1000, 1000);
	// 	float y = FMath::RandRange(-1000, 1000);
	// 	float z = FMath::RandRange(0.0f, 50.0f);
	// 	RandLoc = FVector(x, y, z);
	// 	CurrentTime = 0;
	// }
	//
	// FHitResult Hit;
	// FCollisionQueryParams Params;
	// Params.AddIgnoredActor(this);
	// FVector Start = GetActorLocation() + RandLoc + FVector(0,0,2000);
	// FVector End = Start + FVector(0,0,-10000);
	//
	// bool bHit = GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Params);
	// if(bHit)
	// {
	// 	HitLoc = Hit.ImpactPoint;
	// 	DrawDebugLine(GetWorld(),Start,HitLoc,FColor::Blue,false,1.0f);
	// 	DrawDebugSphere(GetWorld(),HitLoc,30,10,FColor::Blue,false,1.0f);
	// }

	// CurrentTime += DeltaTime;
	// if (CurrentTime > 2)
	// {
	// 	UE_LOG(LogTemp,Warning,TEXT("MonsterSpwan 실행중"));
	// 	float x = FMath::RandRange(-1000,1000);
	// 	float y = FMath::RandRange(-1000,1000);
	// 	float z = FMath::RandRange(0,100);
	// 	
	// 	int32 RandMonsterIndex = FMath::RandRange(0, MonsterFactory.Num() - 1);
	// 	AMonster* Monster = GetWorld()->SpawnActor<AMonster>(MonsterFactory[RandMonsterIndex]);
	// 	
	// 	Monster->SetActorLocation(GetActorLocation() + FVector(x,y,z));
	// 	DrawDebugSphere(GetWorld(),GetActorLocation() + FVector(x,y,z),20,20,FColor::Red);
	// 	
	// 	UE_LOG(LogTemp,Warning,TEXT("MonSpawn 어디서 떨어지냐 %f %f %f"),x,y,z);
	// 	UE_LOG(LogTemp,Warning,TEXT("MonSpawn 몬스터는 어디서 떨어지냐 %f %f %f"),Monster->GetActorLocation().X,
	// 		Monster->GetActorLocation().Y,Monster->GetActorLocation().Z);
	// 	
	// 	Monsters.Add(Monster);
	// 	CurrentTime = 0;
	// }


	
	// 몬스터 생성 타이머
	 // MyTimer([this]()
	 // {
	 // 	if (! HasAuthority()) return;
	 // 	if (Monsters.Num() > 2) return;
	 // 	
	 // 	UE_LOG(LogTemp,Warning,TEXT("MonsterSpwan 실행중"));
	 // 	float x = FMath::RandRange(-1000,1000);
	 // 	float y = FMath::RandRange(-1000,1000);
	 // 	float z = FMath::RandRange(0,100);
	 // 	
	 // 	int32 RandMonsterIndex = FMath::RandRange(0, MonsterFactory.Num() - 1);
	 // 	AMonster* Monster = GetWorld()->SpawnActor<AMonster>(MonsterFactory[RandMonsterIndex]);
	 // 	
	 // 	Monster->SetActorLocation(GetActorLocation() + FVector(x,y,z));
	 // 	DrawDebugSphere(GetWorld(),GetActorLocation() + FVector(x,y,z),20,20,FColor::Red);
	 // 	
	 // 	Monsters.Add(Monster);
	 // 	
	 // 	if (bTimer == true)
	 // 	{
	 // 		UE_LOG(LogTemp, Warning, TEXT("AMonSpawn:: 타이머 몇마리 소환됨 ?? / %d"),Monsters.Num());
	 // 	}
	 // }, 2);
}

void AMonSpawn::MyTimer(void(AMonSpawn::* Func)(), float Second)
{
	bTimer = false;
	
	CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime >= Second)
	{
		(this->*Func)();
		bTimer = true;
		CurrentTime = 0.0f;
	}
}

void AMonSpawn::MyTimer(TFunction<void()> Func, float Second)
{
	bTimer = false;
	CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime >= Second)
	{
		bTimer = true;
		Func();
		CurrentTime = 0.0f;
	}
}

FVector AMonSpawn::RandLocation(float X, float Y, float Z)
{
	float x = FMath::RandRange(-X, X);
	float y = FMath::RandRange(-Y, Y);
	float z = FMath::RandRange(0.0f, Z);
	return FVector(x, y, z);
}

void AMonSpawn::SpawnMonster()
{
	MyTimer([this]()
	{
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		FVector Start = GetActorLocation() + RandLocation() + FVector(0, 0, 1500);
		FVector End = Start + FVector(0,0, -10000);
	
		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start,End,ECC_Visibility,Params);

		if (bHit)
		{
			DrawDebugLine(GetWorld(),Start,Hit.ImpactPoint,FColor::Red,false,1.5f);
			DrawDebugSphere(GetWorld(),Hit.ImpactPoint,20,10,FColor::Blue,false,1.5f);
			
			if (Monsters.Num() > 3) return;
			
			int32 index = FMath::RandRange(0, MonsterFactory.Num());
			AMonster* Monster = GetWorld()->SpawnActor<AMonster>(MonsterFactory[index]);
			Monster->SetActorLocation(Hit.ImpactPoint);
			Monsters.Add(Monster);
		}
	},1.5);
}

