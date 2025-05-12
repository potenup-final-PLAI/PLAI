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
							// Item->SetActorLocation(MyActor->GetActorLocation() + MyActor->GetActorRightVector() * 100);
						}
						else if (ItemStructTable->ItemIndex == 1)
						{
							Item->AttachToComponent(Player->meshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Chest"));
						}
						else if (ItemStructTable->ItemIndex == 2)
						{
							Item->AttachToComponent(Player->meshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("headSocket"));
						} 
						else if (ItemStructTable->ItemIndex == 3)
						{
							Item->AttachToComponent(Player->meshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Chest"));
						}

						// Item->StaticMesh->SetStaticMesh(ItemStructTable->StaticMesh);
						// if (ItemStructTable->ItemIndex == 0)
						// {
						// 	Item->SetActorLocation(MyActor->GetActorLocation() + MyActor->GetActorRightVector() * 100);
						// }
						// else if (ItemStructTable->ItemIndex == 1)
						// {
						// 	Item->SetActorLocation(MyActor->GetActorLocation() + MyActor->GetActorForwardVector() * 100);
						// 	Item->SetActorRotation(Item->GetActorRotation() + FRotator(0, 180, 0));
						// }
						// else if (ItemStructTable->ItemIndex == 2)
						// {
						// 	Item->SetActorLocation(MyActor->GetActorLocation() +FVector(0, 0, 125));
						// } 
						// else if (ItemStructTable->ItemIndex == 3)
						// {
						// 	Item->SetActorLocation(MyActor->GetActorLocation() + MyActor->GetActorForwardVector() * -100);
						// 	Item->SetActorRotation(Item->GetActorRotation() + FRotator(0, 180, 0));
						// }

						
						
						Item->AttachToActor(MyActor,FAttachmentTransformRules::KeepWorldTransform);
						Item->StaticMesh->SetMaterial(0,ItemStructTable->Material);
						// Item->StaticMesh->SetVisibility(false);
					}
				}
			}
		}
	}
}
