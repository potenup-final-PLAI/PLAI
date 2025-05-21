#include "Ruins.h"

#include "KHACharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ARuins::ARuins()
{
	PrimaryActorTick.bCanEverTick = false;
	
	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(RootComponent);
	DetectionSphere->InitSphereRadius(300.f);
	DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);


	//위젯
	RuinsNameWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("BuildingNameText_1"));
	RuinsNameWidget->SetupAttachment(RootComponent);
	RuinsNameWidget->SetWidgetSpace(EWidgetSpace::Screen);
	RuinsNameWidget->SetDrawSize(FVector2D(200, 50));
	RuinsNameWidget->SetVisibility(false); // 처음엔 안 보이게
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/KHA/UI/OpenWorld/WBP_Ruins.WBP_Ruins'")); // 경로 확인 필요
	if (WidgetClass.Succeeded())
	{
		RuinsWidgetClass = WidgetClass.Class;
		RuinsNameWidget->SetWidgetClass(RuinsWidgetClass);
	}

	
}

void ARuins::BeginPlay()
{
	Super::BeginPlay();
	
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ARuins::OnPlayerEnter);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &ARuins::OnPlayerExit);

	if (RuinsNameWidget && RuinsWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("위젯이 정상적으로 초기화되었습니다."));
		RuinsNameWidget->SetVisibility(false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("위젯 초기화 실패 - %s, %s"),
			RuinsNameWidget ? TEXT("위젯 있음") : TEXT("위젯 없음"),
			RuinsWidgetClass ? TEXT("위젯 클래스 있음") : TEXT("위젯 클래스 없음"));
	}
}

void ARuins::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}
void ARuins::OnPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//AKHACharacter* Player = Cast<AKHACharacter>(OtherActor);
	if (AKHACharacter* Player = Cast<AKHACharacter>(OtherActor))
	{
		if (RuinsNameWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("건물 감지됨 - UI 표시"));
			RuinsNameWidget->SetVisibility(true);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("건물 감지됨 - 위젯이 존재하지 않음"));
		}
	}
}

void ARuins::OnPlayerExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	AKHACharacter* Player = Cast<AKHACharacter>(OtherActor);
	if (Cast<AKHACharacter>(OtherActor) && RuinsNameWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("건물 감지안됨"));
		RuinsNameWidget->SetVisibility(false);
	}
}
