// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenComp.h"

#include "PLAI/Item/TestPlayer/TestPlayer.h"


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
	APlayerController* PC = Cast<APlayerController>(GetOwner()->GetWorld()->GetFirstPlayerController());
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

	if (TestPlayer->GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::One))
	{
		UE_LOG(LogTemp,Warning,TEXT("UInvenComp:: IKey누르는중"));
		ItemInvenTory();
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("UInvenComp:: 컨트롤러없음??"));
	}
}

void UInvenComp::ItemInvenTory()
{
	if (Flipflop == false)
	{
		UE_LOG(LogTemp,Warning,TEXT("UInvenComp::ItemInvenTory() 켯다"));
		MenuInven->WBP_MenuInven->SetVisibility(ESlateVisibility::Visible);
		Flipflop = true;
	}
	if (Flipflop == true)
	{
		UE_LOG(LogTemp,Warning,TEXT("UInvenComp::ItemInvenTory() 껏다"));
		MenuInven->WBP_MenuInven->SetVisibility(ESlateVisibility::Hidden);
		Flipflop = false;
	}
}

