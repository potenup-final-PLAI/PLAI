// Fill out your copyright notice in the Description page of Project Settings.


#include "KHA/NPC2.h"

#include "KHACharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ANPC2::ANPC2()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(RootComponent);
	DetectionSphere->InitSphereRadius(300.f);
	DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

// Called when the game starts or when spawned
void ANPC2::BeginPlay()
{
	Super::BeginPlay();

	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ANPC2::OnPlayerEnter);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &ANPC2::OnPlayerExit);
}

// Called every frame
void ANPC2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

// Called to bind functionality to input
void ANPC2::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	

}

void ANPC2::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);

	UE_LOG(LogTemp, Warning, TEXT("NPC2 클릭됨 → 카메라 줌인"));

	AKHACharacter* Player = Cast<AKHACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Player && Player->GetCameraBoom())
	{
		Player->GetCameraBoom()->TargetArmLength = 300.f;  // 거리만 줄여줌
		//Player->bZoomedIn = true;
	}
}

void ANPC2::OnPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AKHACharacter* Player = Cast<AKHACharacter>(OtherActor);
	if (Player)
	{
		Player->bZoomedIn = true;
		UE_LOG(LogTemp, Warning, TEXT("NPC2 감지됨 → 카메라 줌인!"));
	}
}

void ANPC2::OnPlayerExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	AKHACharacter* Player = Cast<AKHACharacter>(OtherActor);
	if (Player)
	{
		Player->bZoomedIn = false;
		UE_LOG(LogTemp, Warning, TEXT("NPC2 범위 벗어남 → 카메라 줌아웃"));
	}
}

