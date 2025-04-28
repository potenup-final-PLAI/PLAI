// Fill out your copyright notice in the Description page of Project Settings.


#include "MonSpawn.h"

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
	UE_LOG(LogTemp,Warning,TEXT("MonsterSpwan 생성됨"));
}

// Called every frame
void AMonSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//몬스터 생성 타이머
	MyTimer([this]()
	{
		if (Monsters.Num() > 10){UE_LOG(LogTemp,Warning,TEXT("MonsterSpwan 그만만들라"))}
		UE_LOG(LogTemp,Warning,TEXT("MonsterSpwan 실행중"));
		
		int32 RandMonsterIndex = FMath::RandRange(0, MonsterFactory.Num() - 1);
		
		AMonster* Monster = GetWorld()->SpawnActor<AMonster>(MonsterFactory[RandMonsterIndex]);
		
		float x = FMath::RandRange(-1000,1000);
        float y = FMath::RandRange(-1000,1000);
        float z = FMath::RandRange(0,100);
		
		Monster->SetActorLocation(FVector(x,y,z));
		Monsters.Add(Monster);
		if (bTimer == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("AMonSpawn:: 타이머 false"));
		}
	}, 3);

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

