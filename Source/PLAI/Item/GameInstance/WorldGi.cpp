// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGi.h"

void UWorldGi::EquipActor(AActor* MyActor)
{
	if (MyActor)
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
						
						Item->StaticMesh->SetStaticMesh(ItemStructTable->StaticMesh);
						if (ItemStructTable->ItemIndex == 0)
						{
							Item->SetActorLocation(MyActor->GetActorLocation() + MyActor->GetActorRightVector() * 100);
						}
						else if (ItemStructTable->ItemIndex == 1)
						{
							Item->SetActorLocation(MyActor->GetActorLocation() + FVector(0,0,-10));
							Item->SetActorScale3D(FVector(0.95,0.95,0.95));
							Item->SetActorRotation(Item->GetActorRotation() + FRotator(0, 180, 0));
						}
						else if (ItemStructTable->ItemIndex == 2)
						{
							Item->SetActorLocation(MyActor->GetActorLocation() +FVector(0, 0, 125));
						} 
						else if (ItemStructTable->ItemIndex == 3)
						{
							Item->SetActorLocation(MyActor->GetActorLocation() + FVector(0, 10, -70));
							Item->SetActorRotation(Item->GetActorRotation() + FRotator(0, 180, 0));
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
