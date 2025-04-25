// Fill out your copyright notice in the Description page of Project Settings.


#include "KHA/MonsterCharacter.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMonsterCharacter::AMonsterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 천천히 걷게 설정
	GetCharacterMovement()->MaxWalkSpeed = 150.f;

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Detection"));
	DetectionSphere->InitSphereRadius(300.f);
	DetectionSphere->SetupAttachment(RootComponent);
	DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	DetectionSphere->SetGenerateOverlapEvents(true);

	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AMonsterCharacter::OnPlayerOverlap);
}

// Called when the game starts or when spawned
void AMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 3초마다 자동 이동 반복
	GetWorldTimerManager().SetTimer(PatrolTimer, this, &AMonsterCharacter::MoveToRandomLocation, 3.0f, true);

	// 첫 이동
	MoveToRandomLocation();

	// 타이머로 이동 반복
	GetWorldTimerManager().SetTimer(PatrolTimer, this, &AMonsterCharacter::MoveToRandomLocation, 3.0f, true);
	MoveToRandomLocation();

	// 캡슐에서 오버랩 감지 연결
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMonsterCharacter::OnPlayerOverlap);

}

// Called every frame
void AMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMonsterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMonsterCharacter::MoveToRandomLocation()
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController) return;

	FNavLocation RandomLocation;
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

	if (NavSystem && NavSystem->GetRandomPointInNavigableRadius(GetActorLocation(), 500.f, RandomLocation))
	{
		AIController->MoveToLocation(RandomLocation.Location);
	}
}

void AMonsterCharacter::OnPlayerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && OtherActor->IsA(ACharacter::StaticClass()))
	{
		// AI 이동 정지
		if (AAIController* AI = Cast<AAIController>(GetController()))
		{
			AI->StopMovement();
		}

		// 물리적으로 멈춤
		GetCharacterMovement()->StopMovementImmediately();

		// 순찰 타이머 중지
		GetWorldTimerManager().ClearTimer(PatrolTimer);

		UE_LOG(LogTemp, Warning, TEXT("플레이어 감지! 몬스터 이동 완전히 중지됨!"));
	}
	bPlayerIsOverlapping = true;
}
