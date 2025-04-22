// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC.h"

#include "Components/SphereComponent.h"


// Sets default values
ANPC::ANPC()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 감지용 SphereComponent 생성
	PlayerDetectRange = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerDetectRange"));
	PlayerDetectRange->SetupAttachment(RootComponent);
	PlayerDetectRange->SetSphereRadius(300.f);  // 감지 반경 설정
	PlayerDetectRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PlayerDetectRange->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	PlayerDetectRange->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PlayerDetectRange->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
	Super::BeginPlay();

	PlayerDetectRange->OnComponentBeginOverlap.AddDynamic(this, &ANPC::OnPlayerEnterRange);
	PlayerDetectRange->OnComponentEndOverlap.AddDynamic(this, &ANPC::OnPlayerExitRange);
	
}

// Called every frame
void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ANPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	
}

void ANPC::OnPlayerEnterRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacter* Player = Cast<ACharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("플레이어 감지됨: %s"), *Player->GetName());

		UE_LOG(LogTemp, Warning, TEXT("NPC: 안녕")); // NPC가 먼저 인사
		bPlayerInRange = true;
	}
}

void ANPC::OnPlayerExitRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacter* Player = Cast<ACharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("플레이어가 영역을 벗어남: %s"), *Player->GetName());

		bPlayerInRange = false;
	}
}

void ANPC::Interact(ACharacter* Interactor)
{
	UE_LOG(LogTemp,Warning,TEXT("플레이어:안녕하세요"));
}

void ANPC::InteractTurnBased(ACharacter* Interactor)
{
	if (bNpcTrun)
	{
		UE_LOG(LogTemp, Warning, TEXT("NPC: 안녕"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("플레이어: 안녕하세요"));
	}

	bNpcTrun = !bNpcTrun;  // 번갈아가며 바뀜
}

bool ANPC::IsPlayerInRange() const
{
	return bPlayerInRange;
}

// bool ANPC::IsPlayerInRange() const
// {
// 	return true;
// }
