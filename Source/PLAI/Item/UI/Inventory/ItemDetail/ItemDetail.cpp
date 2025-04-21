// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDetail.h"

#include "Components/TextBlock.h"
#include "PLAI/Item/Item/ItemStruct.h"

void UItemDetail::SetItemDetail(const FItemStruct& ItemStruct)
{
	if (ItemStruct.ItemTop != -1)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemDetail 아이템 구조체 정보%s %s"),*ItemStruct.Name,*ItemStruct.NameType)
		Name->SetText(FText::FromString(ItemStruct.Name));
		NameType->SetText(FText::FromString(ItemStruct.NameType));
		NameDetail->SetText(FText::FromString(ItemStruct.NameDetail));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ItemDetail::SetItemDetail: Invalid Item Struct"));
	}
}
