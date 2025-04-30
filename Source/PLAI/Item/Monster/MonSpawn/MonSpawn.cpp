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

	// UIChaMain = CreateWidget<UUIChaMain>(GetWorld(),UUIChaMainFactory);
	// UIChaMain->AddToViewport();
	
	UE_LOG(LogTemp,Warning,TEXT("MonsterSpwan 생성됨"));
}

// Called every frame
void AMonSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// CurrentTime += DeltaTime;
	
	if (CurrentTime > 2)
	{
		UE_LOG(LogTemp,Warning,TEXT("MonsterSpwan 실행중"));
		float x = FMath::RandRange(-1000,1000);
		float y = FMath::RandRange(-1000,1000);
		float z = FMath::RandRange(0,100);
		
		int32 RandMonsterIndex = FMath::RandRange(0, MonsterFactory.Num() - 1);
		AMonster* Monster = GetWorld()->SpawnActor<AMonster>(MonsterFactory[RandMonsterIndex]);
		
		Monster->SetActorLocation(GetActorLocation() + FVector(x,y,z));
		DrawDebugSphere(GetWorld(),GetActorLocation() + FVector(x,y,z),20,20,FColor::Red);
		
		UE_LOG(LogTemp,Warning,TEXT("MonSpawn 어디서 떨어지냐 %f %f %f"),x,y,z);
		UE_LOG(LogTemp,Warning,TEXT("MonSpawn 몬스터는 어디서 떨어지냐 %f %f %f"),Monster->GetActorLocation().X,
			Monster->GetActorLocation().Y,Monster->GetActorLocation().Z);
		
		Monsters.Add(Monster);
		CurrentTime = 0;
	}

	//몬스터 생성 타이머
	// MyTimer([this]()
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
	// 	
	// 	Monsters.Add(Monster);
	// 	
	// 	if (bTimer == true)
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("AMonSpawn:: 타이머 true"));
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

