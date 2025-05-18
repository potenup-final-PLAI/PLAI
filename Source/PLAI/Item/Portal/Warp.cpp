// Fill out your copyright notice in the Description page of Project Settings.


#include "Warp.h"

#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PLAI/Item/GameInstance/WorldGi.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Portal/UiPortal.h"


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
	
	WarpLocation.Add(FVector(0,0,0));
	WarpLocation.Add( FVector(-1000000,0,0));
	WarpLocation.Add(FVector(0,1000000,0));
	WarpLocation.Add(FVector(0,-1000000,0));

	SphereComp->OnComponentBeginOverlap.AddDynamic(this,&AWarp::OnOverlappedWarp);
	SphereComp->OnComponentEndOverlap.AddDynamic(this,&AWarp::OnEndOvelappedWarp);
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
		if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(OtherActor))
		{
			// if (TestPlayer->HasAuthority() && TestPlayer->IsLocallyControlled())
			if (TestPlayer->IsLocallyControlled())
			{
				UiPortal = CreateWidget<UUiPortal>(GetWorld(),UiPortalFactory);
				UiPortal->AddToViewport(5);
				UiPortal->TestPlayer = TestPlayer;
				UiPortal->Warp = this;
				UiPortal->WorldMap->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void AWarp::OnEndOvelappedWarp(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(OtherActor))
		{
			// if (TestPlayer->HasAuthority() && TestPlayer->IsLocallyControlled())
			if (TestPlayer->IsLocallyControlled())
			{
				if (!UiPortal){UE_LOG(LogTemp,Warning,TEXT("Warp에 유아이포탈없음")) return;};
				UiPortal->TestPlayer = nullptr;
				UiPortal->RemoveFromParent();
			}
			// if (!UiPortal->TestPlayer){UE_LOG(LogTemp,Warning,TEXT("Warp에 Player 없음")) return;};
		}
	}
}

void AWarp::Server_WarpPlayer_Implementation(ATestPlayer* TestPlayer)
{
	SetOwner(TestPlayer);
	UE_LOG(LogTemp,Warning,TEXT("Warp ServerWarpPlayer 실행"))
	TestPlayer->SetActorLocation(GetActorLocation() + FVector(0,0,2000));
}



void AWarp::WarpLevel(class ATestPlayer* TestPlayer, int32 index)
{
	UWorldGi* Gi = Cast<UWorldGi>(GetWorld()->GetGameInstance());
	
	bool bSpawn = false;
	for (int i = 0; i < Gi->bWorldSpawnInt.Num(); i++)
	{
		if (Gi->bWorldSpawnInt[i] == index)
		{
			UE_LOG(LogTemp, Display, TEXT("Warp Spawned %d"),index);
			bSpawn = true;
			break;
		}
	}
	if (bSpawn == false)
	{
		if (index == 0){UE_LOG(LogTemp,Warning,TEXT("Warp Village 소환 XX"))}
		else{
			bool bSuccess = false; // 성공 여부
			// 레벨을 동적으로 로드
			ULevelStreamingDynamic* OldLevelStream = ULevelStreamingDynamic::LoadLevelInstance(GetWorld(),
				OldLevelPath[index],WarpLocation[index],FRotator(0,0,0),bSuccess);
			ULevelStreamingDynamic* NewLevelStream = ULevelStreamingDynamic::LoadLevelInstance(GetWorld(),
				NewLevelPath[index],WarpLocation[index],FRotator(0,0,0),bSuccess);

			if (bSuccess)
			{ UE_LOG(LogTemp, Log, TEXT("Level loaded successfully!")); }
			else
			{ UE_LOG(LogTemp, Log, TEXT("Failed to load level."));}
			Gi->bWorldSpawnInt.Add(index);
		}
	}
	if (!TestPlayer){ UE_LOG(LogTemp, Log, TEXT("Walp TestPlayer 없음."));}
	FTimerHandle TimerHandle;
	FVector TargetLocation = WarpLocation[index] + FVector(0,0,400);
	ATestPlayer* TargetPlayer = UiPortal->TestPlayer; // WarpPlayer도 캡쳐해줘야 안정적
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [TargetPlayer, TargetLocation]()
	{
		if (TargetPlayer)
		{
			TargetPlayer->SetActorLocation(TargetLocation);
			FVector Forward = TargetPlayer->CameraBoom->GetForwardVector(); // 전방 방향
		}
	}, 3.5f, false);
}






// FString StreamingLevelPath = TEXT("/Game/Mk_Item/Mk_LevelDessert.Mk_LevelDessert");
//
// // 동적으로 로드한 스트리밍 레벨을 Unload
// ULevelStreamingDynamic* StreamingLevel = ULevelStreamingDynamic::GetStreamingLevel(GetWorld(), FName(*StreamingLevelPath));
//
// if (StreamingLevel)
// {
// 	StreamingLevel->UnloadLevelInstance();
// 	UE_LOG(LogTemp, Warning, TEXT("동적 스트리밍 레벨 언로드 성공: %s"), *StreamingLevelPath);
// }
// else
// {
// 	UE_LOG(LogTemp, Error, TEXT("동적 스트리밍 레벨을 찾을 수 없습니다: %s"), *StreamingLevelPath);
// }

// bool bShouldLoad = false;
// // 어디서든 호출
// FVector SpawnLocation(0.f, 0.f, 0.f); // Spawn 위치
// FRotator SpawnRotation(0.f, 0.f, 0.f); // Spawn 회전
// // OldLevel을 NewLevel에서 스트리밍
// ULevelStreamingDynamic* OldLevelStreaming = ULevelStreamingDynamic::LoadLevelInstance(GetWorld(),
// 	OldPathVillage,SpawnLocation,SpawnRotation,bShouldLoad);
// ULevelStreamingDynamic* NewLevelStreaming = ULevelStreamingDynamic::LoadLevelInstance(GetWorld(),
// NewPathVillage,SpawnLocation,SpawnRotation,bShouldLoad);
	
// NewLevel을 로드
// if (bShouldLoad)
// {UE_LOG(LogTemp, Display, TEXT("Wrap OnOverlappedWarp: 레벨 이동 시작"));
// 	// 레벨을 오픈 (현재 월드 새로 로드)
// 	UGameplayStatics::OpenLevel(GetWorld(), LevelName);}
// else
// {    // OpenLevel은 레벨을 열기만 하지 "언로드" 개념은 없음
// 	// 필요하면 여기서 따로 처리를 추가해야 함
// 	UE_LOG(LogTemp, Warning, TEXT("Wrap OnOverlappedWarp: 레벨 언로드는 OpenLevel 방식에선 별도 처리 필요"));
// }


