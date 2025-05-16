// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGi.h"

#include "Player/BattlePlayer.h"

void UWorldGi::EquipActor(AActor* MyActor)
{
	if (ABattlePlayer* Player = Cast<ABattlePlayer>(MyActor))
	{
		for (int32 i = 0; i < UserFullInfoGiStat.equipment_info.item_list.Num(); i++)
		{
			if (AItemMaster* Item = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory))
			{
				TArray<FName>Raws = ItemDataTable->GetRowNames();
				for (FName Raw : Raws)
				{
					if (Raw == UserFullInfoGiStat.equipment_info.item_list[i].item_id)
					{
						FRotator Rotator = MyActor->GetActorRotation();
						
						FItemStructTable* ItemStructTable = ItemDataTable->FindRow<FItemStructTable>(Raw,TEXT("WorldGi"));
						// 여기부터 장비 장착
						Item->StaticMesh->SetStaticMesh(ItemStructTable->StaticMesh);
						if (ItemStructTable->ItemIndex == 0)
						{
							Item->AttachToComponent(Player->meshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("hand_rSocket"));
							Item->AddActorWorldOffset(FVector(30, 5, -10));
							Item->AddActorLocalRotation(FRotator(-165, 10, 100));
						}
						else if (ItemStructTable->ItemIndex == 1)
						{
							Item->AttachToComponent(Player->meshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Chest"));
							Item->AddActorWorldOffset(FVector(-20, 0, -80));
							Item->SetActorRotation(Item->GetActorRotation() + FRotator(0, 90, 0));
							Item->SetActorScale3D(FVector(1.05, 1.05, 1.05));
						}
						else if (ItemStructTable->ItemIndex == 2)
						{
							Item->AttachToComponent(Player->meshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("headSocket"));

							Item->AddActorWorldOffset(FVector(-0, 0, 10));
							
							Item->AddActorLocalRotation(FRotator(0, -90, 90));
							
							Item->SetActorScale3D(FVector(0.8,0.8,0.8));
						} 
						else if (ItemStructTable->ItemIndex == 3)
						{
							Item->AttachToComponent(Player->meshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("FX_Ult_Passive"));

							Item->AddActorWorldOffset(FVector(-0, 0, -130));
							Item->AddActorLocalRotation(FRotator(0, 100, 0));
						}
						Item->StaticMesh->SetMaterial(0,ItemStructTable->Material);
					}
				}
			}
		}
	}
}
