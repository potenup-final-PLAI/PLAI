// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreComp.h"

#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
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
		StoreInven->AddToViewport();
		StoreInven->SetVisibility(ESlateVisibility::Hidden);
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

void UStoreComp::SetStoreInven(const FItemStructsArray& ItemStructsArray)
{
	for (UWidget* Widget : StoreInven->WrapBox->GetAllChildren())
	{
		USlotStore* Slot = Cast<USlotStore>(Widget);
		int32 index = StoreInven->WrapBox->GetChildIndex(Slot);
		if (index > ItemStructsArray.ItemStructs.Num()-1)
		{ break; }
		Slot->ItemStruct = ItemStructsArray.ItemStructs[index];
		Slot->SlotImageUpdate();
	}
}

