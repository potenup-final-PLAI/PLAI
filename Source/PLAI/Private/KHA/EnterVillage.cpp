// Fill out your copyright notice in the Description page of Project Settings.


#include "KHA/EnterVillage.h"

#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnterVillage::AEnterVillage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->InitSphereRadius(300.f);  // 범위: 3m 반경
	TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSphere->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = TriggerSphere;
}

// Called when the game starts or when spawned
void AEnterVillage::BeginPlay()
{
	Super::BeginPlay();
	
	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnterVillage::OnPlayerEnter);
}

// Called every frame
void AEnterVillage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AEnterVillage::OnPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* PlayerChar = Cast<ACharacter>(OtherActor);
	if (PlayerChar)
	{
		FVector CurrentLocation = PlayerChar->GetActorLocation();
		FVector NewLocation = CurrentLocation + FVector(500.f,500.f,0.f); 
		PlayerChar->SetActorLocation(NewLocation);
		
		
		PlayerChar->GetCharacterMovement()->StopMovementImmediately(); //이동 멈춤
		UE_LOG(LogTemp, Warning, TEXT("EnterVillage 감지됨 → 캐릭터 이동"));
	}
}


