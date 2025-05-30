// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDetail.h"

#include "GameDelegates.h"
#include "Components/Image.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "PLAI/Item/Item/ItemMaster.h"
#include "PLAI/Item/Item/ItemStruct.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemInven.h"

void UItemDetail::SetItemDetail(const FItemStructTable& ItemStruct)
{
	if (ItemStruct.ItemTop == -1){UE_LOG(LogTemp,Warning,TEXT("SetItemDetail없음"));return;}
	if (ItemStruct.ItemTop != -1)
	{
		// UE_LOG(LogTemp, Error, TEXT("ItemDetail 아이템 구조체 정보%s %s"),*ItemStruct.Name,*ItemStruct.NameType)
		Name->SetText(FText::FromString(ItemStruct.Name));
		NameType->SetText(FText::FromString(ItemStruct.NameType));
		NameDetail->SetText(FText::FromString(ItemStruct.NameDetail));
		FSlateBrush Brush;
		Brush.SetResourceObject(ItemStruct.Texture);
		IconImage->SetBrush(Brush);
		// FString Index = FString::Printf(TEXT("T[%d] I[%d] Y[%d] D[%d]"), ItemStruct.ItemTop, ItemStruct.ItemIndex, 
	 //    ItemStruct.ItemIndexType, ItemStruct.ItemIndexDetail);
		// StatE->SetText(FText::FromString(Index));
		StatD->SetText(FText::AsNumber(ItemStruct.ItemGold));
		
		// StatNameE->SetText(FText::AsNumber(ItemStruct.ItemStructStat.Item_CRIT));
		StatNameC->SetText(FText::FromString(ItemStruct.ItemStructStatName.Item_CRIT));
		
        if (ItemStruct.ItemTop == 1 && ItemStruct.ItemIndex == 0)
        {// 공격아이템
	        StatA->SetText(FText::AsNumber(ItemStruct.ItemStructStat.item_ATK));
        	StatNameA->SetText(FText::FromString(ItemStruct.ItemStructStatName.item_ATK));

        	StatB->SetText(FText::AsNumber(ItemStruct.ItemStructStat.item_CRITDMG));
        	StatNameB->SetText(FText::FromString(ItemStruct.ItemStructStatName.item_CRITDMG));

        	StatC->SetText(FText::AsNumber(ItemStruct.ItemStructStat.Item_CRIT));
        	StatNameC->SetText(FText::FromString(ItemStruct.ItemStructStatName.Item_CRIT));
        }// 방어아이템
		if (ItemStruct.ItemTop == 1 && ItemStruct.ItemIndex == 1 || ItemStruct.ItemIndex == 2 || ItemStruct.ItemIndex == 3)
		{
			StatA->SetText(FText::AsNumber(ItemStruct.ItemStructStat.item_DEF));
			StatNameA->SetText(FText::FromString(ItemStruct.ItemStructStatName.item_DEF));

			StatB->SetText(FText::AsNumber(ItemStruct.ItemStructStat.item_RES));
			StatNameB->SetText(FText::FromString(ItemStruct.ItemStructStatName.item_RES));

			StatC->SetText(FText::AsNumber(ItemStruct.ItemStructStat.item_SHI));
			StatNameC->SetText(FText::FromString(ItemStruct.ItemStructStatName.item_SHI));
		}
		// 소비아이템
		if (ItemStruct.ItemTop == 0 || ItemStruct.ItemIndex == 0 || ItemStruct.ItemIndex == 0)
		{
			StatA->SetText(FText::AsNumber(ItemStruct.ItemStructStat.item_ATK));
			StatNameA->SetText(FText::FromString(ItemStruct.ItemStructStatName.item_ATK));

			StatB->SetText(FText::AsNumber(ItemStruct.ItemStructStat.item_RES));
			StatNameB->SetText(FText::FromString(ItemStruct.ItemStructStatName.item_RES));

			StatC->SetText(FText::AsNumber(ItemStruct.ItemStructStat.item_SHI));
			StatNameC->SetText(FText::FromString(ItemStruct.ItemStructStatName.item_SHI));
		}
	}
}
