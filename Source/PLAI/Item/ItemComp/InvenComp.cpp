// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenComp.h"

#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemInven.h"


// Sets default values for this component's properties
UInvenComp::UInvenComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInvenComp::BeginPlay()
{
	Super::BeginPlay();
	MenuInven = CreateWidget<UMenuInven>(GetWorld(),MenuInvenFactory);
	MenuInven->AddToViewport();
	MenuInven->SetVisibility(ESlateVisibility::Hidden);
	
	TestPlayer = Cast<ATestPlayer>(GetOwner());
	if (TestPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInvenComp::플레이어 BeginPlay()"));
	}
    else
    {
    	UE_LOG(LogTemp, Warning, TEXT("UInvenComp::플레이어없음 BeginPlay()"));
    }
	
	PC = Cast<APlayerController>(GetOwner()->GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInvenComp: 컨트롤러 있음 BeginPlay()"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInvenComp: 컨트롤러 없음 BeginPlay()"));
	}
	// ...
	
}


// Called every frame
void UInvenComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (PC && PC->IsLocalController())
	{
		if (PC->WasInputKeyJustPressed(EKeys::I))
		{
			UE_LOG(LogTemp, Warning, TEXT("컴포넌트에서 입력 받음!"));
			ItemInvenTory();
		}
	}
}

void UInvenComp::ItemInvenTory()
{
	if (Flipflop == false)
	{
		UE_LOG(LogTemp,Warning,TEXT("UInvenComp::ItemInvenTory() 켯다"));
		MenuInven->WBP_ItemInven->SetVisibility(ESlateVisibility::Visible);
		Flipflop = true;
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("UInvenComp::ItemInvenTory() 껏다"));
		MenuInven->WBP_ItemInven->SetVisibility(ESlateVisibility::Hidden);
		Flipflop = false;
	}
}

