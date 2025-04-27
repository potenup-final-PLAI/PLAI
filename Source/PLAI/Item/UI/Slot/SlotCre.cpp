// Fill out your copyright notice in the Description page of Project Settings.


#include "SlotCre.h"

#include "Blueprint/DragDropOperation.h"
#include "PLAI/Item/Creture/Creature.h"
#include "PLAI/Item/Item/ItemObject.h"
#include "PLAI/Item/ItemComp/CreComp.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"

FReply USlotCre::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// TArray<FName>RawNames = ItemTable->GetRowNames();
	// for (FName RawName : RawNames)
	// {
	// 	FItemStructTable* Table = ItemTable->FindRow<FItemStructTable>(RawName,TEXT("SlotCre"));
	// 	if (ItemStructTable->ItemIndex)
	// }
	
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

bool USlotCre::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                            UDragDropOperation* InOperation)
{
	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
    if (ItemObject && ItemObject->ItemStructTable.ItemTop == 3
    	&& ItemObject && ItemObject->ItemStructTable.CreatureFactory != nullptr)
    {
	    UE_LOG(LogTemp,Warning,TEXT("USlotCre::크리처 들어왔음 NativeOnDrop"));
    	if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
    	{
    		TestPlayer->CreComp->Creature = GetWorld()->SpawnActor<ACreature>(ItemObject->ItemStructTable.CreatureFactory);
    		TestPlayer->CreComp->Creature->AttachToActor(TestPlayer,FAttachmentTransformRules::KeepRelativeTransform);
    		TestPlayer->CreComp->Creature->SetActorLocation(TestPlayer->GetActorLocation()+FVector(0,125,125));
    	}
    	else
    	{
    		UE_LOG(LogTemp,Warning,TEXT("USlotCre:: TestPlayer캐스팅 실패 NativeOnDrop"));
    	}
    }
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("USlotCre::크리처 안들어왔음 NativeOnDrop"));
		return false;
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}