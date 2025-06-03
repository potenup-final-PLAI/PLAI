// Fill out your copyright notice in the Description page of Project Settings.


#include "EndingPortal.h"

#include "Battle/Util/DebugHeader.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/BoxComponent.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "UI/EndingCredit.h"


// Sets default values
AEndingPortal::AEndingPortal()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("boxComp"));
	SetRootComponent(boxComp);
	boxComp->SetBoxExtent(FVector(50, 200, 200));
	boxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	boxComp->SetCollisionObjectType(ECC_WorldStatic);
	boxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	boxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	boxComp->SetGenerateOverlapEvents(true);
	
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AEndingPortal::BeginPlay()
{
	Super::BeginPlay();

	boxComp->OnComponentBeginOverlap.AddDynamic(this, &AEndingPortal::OnOverlapBegin);
	
}

// Called every frame
void AEndingPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEndingPortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	NET_PRINTLOG(TEXT("OverLap"));
	if (auto* player = Cast<ATestPlayer>(OtherActor))
	{
		NET_PRINTLOG(TEXT("player"));
		if (HasAuthority())
		{
			NET_PRINTLOG(TEXT("server"));
			Multicast_EndingUI();
		}
	}
}

void AEndingPortal::Multicast_EndingUI_Implementation()
{
	// 모든 위젯 제거
	TArray<UUserWidget*> widgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, widgets, UUserWidget::StaticClass(), false);

	for (UUserWidget* widget : widgets)
	{
		if (widget)
		{
			widget->RemoveFromParent();
		}
	}
	
	if (auto* endingCreditUI = CreateWidget<UEndingCredit>(GetWorld(), endingCreditFactory))
	{
		endingCreditUI->AddToViewport();
		endingCreditUI->Mulitcast_PaidIn();
	}
	
}
