// Fill out your copyright notice in the Description page of Project Settings.


#include "SlotQuick.h"

#include "Blueprint/DragDropOperation.h"
#include "PLAI/Item/Item/ItemMaster.h"
#include "PLAI/Item/Item/ItemObject.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"

bool USlotQuick::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                              UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Display, TEXT("SlotQuick::NativeOnDrop"));
	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);

	ATestPlayer* TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
	AItem* Item = GetWorld()->SpawnActor<AItemMaster>(TestPlayer->InvenComp->ItemMasterFactory);
	Item->SetActorLocation(TestPlayer->GetActorLocation() + TestPlayer->GetActorForwardVector() * 75);
	Item->StaticMesh->SetStaticMesh(ItemObject->ItemStructTable.StaticMesh);
	
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
