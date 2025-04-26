// Fill out your copyright notice in the Description page of Project Settings.


#include "Warp.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"


// Sets default values
AWarp::AWarp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
    SetRootComponent(MeshComp);
	
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComponent");
    SphereComp->SetupAttachment(GetRootComponent());
	
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>("BoxComponent");
	NiagaraComp->SetupAttachment(GetRootComponent());
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
	if (OtherActor)
	{
		if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(OtherActor)){}
		{
			WarpLevel(TEXT("LandscapeAutoMaterial_Island_Example"),true);
		}
	}
}

void AWarp::WarpLevel(FName LevelName, bool bShouldLoad)
{
	// 어디서든 호출
	FString LevelPath = TEXT("/Game/AdvancedVillagePack/Maps/AdvancedVillagePack_Showcase.AdvancedVillagePack_Showcase"); // 레벨 경로 (Content 밑부터)
	FVector SpawnLocation(0.f, 0.f, 0.f); // 레벨 생성 위치
	FRotator SpawnRotation(0.f, 0.f, 0.f); // 레벨 생성 회전
	bool bSuccess = false;

	ULevelStreamingDynamic* StreamingLevel = ULevelStreamingDynamic::LoadLevelInstance(
		GetWorld(), LevelPath, SpawnLocation, SpawnRotation, /*Out Success*/ bSuccess);

	if (StreamingLevel)
	{ UE_LOG(LogTemp, Warning, TEXT("동적 스트리밍 레벨 로딩 성공: %s"), *LevelPath); }
	else
	{ UE_LOG(LogTemp, Error, TEXT("동적 스트리밍 레벨 로딩 실패: %s"), *LevelPath); }
	
	// if (bShouldLoad)
	// {UE_LOG(LogTemp, Display, TEXT("Wrap OnOverlappedWarp: 레벨 이동 시작"));
	// 	// 레벨을 오픈 (현재 월드 새로 로드)
	// 	UGameplayStatics::OpenLevel(GetWorld(), LevelName);}
	// else
	// {    // OpenLevel은 레벨을 열기만 하지 "언로드" 개념은 없음
	// 	// 필요하면 여기서 따로 처리를 추가해야 함
	// 	UE_LOG(LogTemp, Warning, TEXT("Wrap OnOverlappedWarp: 레벨 언로드는 OpenLevel 방식에선 별도 처리 필요"));
	// }
}


