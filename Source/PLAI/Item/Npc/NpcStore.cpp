// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcStore.h"

#include "Components/WrapBox.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"


// Sets default values
ANpcStore::ANpcStore()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANpcStore::BeginPlay()
{
	Super::BeginPlay();
	
	Item = ItemFactory->GetDefaultObject<AItem>();

	StoreInven = CreateWidget<UStoreInven>(GetWorld(),StoreInvenFactory);
	StoreInven->AddToViewport(0);
	StoreInven->SetVisibility(ESlateVisibility::Hidden);
	
	NpcUiMaster = Cast<UWidget>(StoreInven);
	NpcNameString = TEXT("토리 (Store)");
}

// Called every frame
void ANpcStore::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ANpcStore::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// void ANpcStore::SetStoreInven()
// {
// 	if (TestPlayer && TestPlayer->IsLocallyControlled())
// 	{
// 		for (UWidget* Widget : StoreInven->WrapBox->GetAllChildren())
// 		{
// 			USlotStore* Slot = Cast<USlotStore>(Widget);
// 			int32 index = StoreInven->WrapBox->GetChildIndex(Slot);
//
// 			TArray<FName>RawNames = ItemTable->GetRowNames();
// 			FItemStructTable* RowData = ItemTable->FindRow<FItemStructTable>(RawNames[index],TEXT("StoreComp"));
// 			Slot->ItemStructTable = *RowData;
// 			Slot->SlotImageUpdate();
//
// 			// if (Slot->ItemStructTable.ItemTop == -1)
// 			// {
// 			// 	UE_LOG(LogTemp,Warning,TEXT("스토어컴프 테이블 복사 셋팅 X %s"),*Slot->ItemStructTable.Name)
// 			// }
// 			// else
// 			// {
// 			// 	UE_LOG(LogTemp,Warning,TEXT("스토어컴프 테이블 복사 셋팅 되었음 %s"),*Slot->ItemStructTable.Name)
// 			// }
// 		}
// 	}
// }

