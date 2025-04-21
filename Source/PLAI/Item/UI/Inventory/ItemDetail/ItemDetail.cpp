// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDetail.h"

#include "GameDelegates.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "PLAI/Item/Item/ItemMaster.h"
#include "PLAI/Item/Item/ItemStruct.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemInven.h"

void UItemDetail::SetItemDetail(const FItemStruct& ItemStruct)
{
	if (ItemStruct.ItemTop != -1)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemDetail 아이템 구조체 정보%s %s"),*ItemStruct.Name,*ItemStruct.NameType)
		Name->SetText(FText::FromString(ItemStruct.Name));
		NameType->SetText(FText::FromString(ItemStruct.NameType));
		Description->SetText(FText::FromString(ItemStruct.NameDetail));

		if (ATestPlayer* Player = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn()))
		{
			if (AItemMaster* ItemMaster = Player->InvenComp->ItemMasterFactory->GetDefaultObject<AItemMaster>())
			{
				if (AItem* Item = ItemMaster->ItemFactory->GetDefaultObject<AItem>())
				{
					UE_LOG(LogTemp, Error, TEXT("ItemDetail 아이템 있음%d"),Item->ItemStructTop.ItemMeshTops.Num());
					FSlateBrush Brush;
					Brush.SetResourceObject(Item->ItemStructTop.ItemMeshTops[ItemStruct.ItemTop].ItemMeshIndexes[ItemStruct.ItemIndex].
					   ItemMeshTypes[ItemStruct.ItemIndexType].Textures[ItemStruct.ItemIndexDetail]);
					IconImage->SetBrush(Brush);
				}
			}
		}
	}
}
