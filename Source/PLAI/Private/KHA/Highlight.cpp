// Fill out your copyright notice in the Description page of Project Settings.


#include "KHA/Highlight.h"

// Sets default values
AHighlight::AHighlight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	// 기본 메시 세팅 (SM_Cube 등)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube"));
	if (MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(MeshAsset.Object);
	}

	// 커서 이벤트 허용
	Mesh->SetGenerateOverlapEvents(true);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetRenderCustomDepth(false);
}

// Called when the game starts or when spawned
void AHighlight::BeginPlay()
{
	Super::BeginPlay();

	Mesh->SetRenderCustomDepth(true);
}

// Called every frame
void AHighlight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHighlight::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();
	Mesh->SetRenderCustomDepth(true);
	UE_LOG(LogTemp, Warning, TEXT("외곽선 켜짐"));
}

void AHighlight::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();
	Mesh->SetRenderCustomDepth(false);
	UE_LOG(LogTemp, Warning, TEXT("외곽선 꺼짐"));
}

