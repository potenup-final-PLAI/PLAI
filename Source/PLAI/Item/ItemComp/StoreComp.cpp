// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreComp.h"

#include "Blueprint/UserWidget.h"
#include "PLAI/Item/UI/Inventory/StoreInven/StoreInven.h"


// Sets default values for this component's properties
UStoreComp::UStoreComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStoreComp::BeginPlay()
{
	Super::BeginPlay();
	if (StoreInvenFactory)
	{
		StoreInven = CreateWidget<UStoreInven>(GetWorld(),StoreInvenFactory);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UStoreComp::BeginPlay 스토인벤 생성불가"));
	}

	
	// ...
	
}


// Called every frame
void UStoreComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UStoreComp::AddStoreInven()
{
	StoreInven->AddToViewport();
}

