// Fill out your copyright notice in the Description page of Project Settings.


#include "Slot.h"

#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PLAI/Item/Item/ItemObject.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Inventory/ItemDetail/ItemDetail.h"


void USlot::SlotCountUpdate(const int32 Count)
{
	SlotCount->SetText(FText::AsNumber(Count));
}

void USlot::SlotImageUpdate()
{
	if (ItemStructTable.ItemTop != -1)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(ItemStructTable.Texture);
		SlotImage->SetBrush(Brush);
	}
	if (ItemStruct.ItemTop == -1)return;
	FSlateBrush Brush;
	Brush.SetResourceObject(ParentItem->ItemStructTop.ItemMeshTops[ItemStruct.ItemTop].ItemMeshIndexes[ItemStruct.ItemIndex].
		ItemMeshTypes[ItemStruct.ItemIndexType].Textures[ItemStruct.ItemIndexDetail]);
	SlotImage->SetBrush(Brush);
	
}

void USlot::NativeConstruct()
{
	Super::NativeConstruct();
	ParentItem = ItemFactory->GetDefaultObject<AItem>();
}

// FItemStruct* USlot::ItemDetailShow()
// {
// 	const TArray<FName> Rownames = ItemTable->GetRowNames();
//
// 	for (const FName& RowName : Rownames)
// 	{
// 		FItemStruct* Row = ItemTable->FindRow<FItemStruct>(RowName, TEXT("ItemTable Slot"));
// 		if (Row && Row->ItemIndex == ItemStruct.ItemIndex
// 			&& Row->ItemTop == ItemStruct.ItemTop
// 			&& Row->ItemIndexType == ItemStruct.ItemIndexType
// 			&& Row->ItemIndexDetail == ItemStruct.ItemIndexDetail)
// 		{
// 			
// 		}
// 	}
// }

FItemStruct* USlot::ItemTableFind()
{
	const TArray<FName> Rownames = ItemTable->GetRowNames();

	for (const FName& RowName : Rownames)
	{
		FItemStruct* Row = ItemTable->FindRow<FItemStruct>(RowName, TEXT("ItemTable Slot"));
		if (Row && Row->ItemIndex == ItemStruct.ItemIndex
			&& Row->ItemTop == ItemStruct.ItemTop
			&& Row->ItemIndexType == ItemStruct.ItemIndexType
			&& Row->ItemIndexDetail == ItemStruct.ItemIndexDetail)
		{
			return Row;
		}
	}
	return nullptr;
}

void USlot::ItemTableShow()
{
	// if (APawn* TestPlayer = GetWorld()->GetFirstPlayerController()->GetPawn())
	// {
	// 	if (TestPlayer->IsLocallyControlled())
	// 	{
	// 		FItemStruct* ItemStructTable = ItemTableFind();
	//         if (ItemStructTable == nullptr){UE_LOG(LogTemp,Warning,TEXT("슬롯 테이블값 없음 리턴")) return;};
	//         ItemStruct = *ItemStructTable;
	//
 //        	UE_LOG(LogTemp, Display, TEXT("USlot::ItemTableShow() 아이템 데이블 구조체 %s%s%s"),*ItemStructTable->Name,*ItemStructTable->NameType,
	//     	*ItemStructTable->NameDetail);
	//
 //        	UE_LOG(LogTemp, Display, TEXT("USlot::ItemTableShow() 아이템 구조체 %s%s%s"),*ItemStruct.Name,*ItemStruct.NameType,
	// 	    *ItemStruct.NameDetail);
	// 	}
	// }
}

FReply USlot::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (ItemStruct.ItemTop == -1)
	{
		UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnMouseButtonDown 슬롯 템 없음"));
		return Super::NativeOnMouseButtonDown(MyGeometry, MouseEvent);
	}

	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		UE_LOG(LogTemp, Display, TEXT("Slot::왼쪽마우스 NativeOnMouseButtonDown"));
		return UWidgetBlueprintLibrary::DetectDragIfPressed(MouseEvent, this,
			EKeys::LeftMouseButton).NativeReply;
	}
	if (MouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		UE_LOG(LogTemp, Display, TEXT("Slot::오른쪽마우스 NativeOnMouseButtonDown"));
		APlayerController* PlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
		
		if (PlayerController->IsLocalController())
		{
			ATestPlayer* TestPlayer = Cast<ATestPlayer>(PlayerController->GetPawn());
			
			bItemDetail = !bItemDetail;
			ItemTableShow();

			// UE_LOG(LogTemp, Display, TEXT("USlot::ItemTableShow() %s %s %s"),*ItemStruct.Name,*ItemStruct.NameType,
			// *ItemStruct.NameDetail);
			
			TestPlayer->InvenComp->MenuInven->WBP_ItemDetail->SetVisibility(bItemDetail ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
			TestPlayer->InvenComp->MenuInven->WBP_ItemDetail->SetItemDetail(ItemStruct);
			
			// FGeometry Geometry = GetCachedGeometry();
			// FVector2D Position = Geometry.GetAbsolutePosition();
			// float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
			// FVector2D ScalePosition = Position / Scale;
			// TestPlayer->InvenComp->MenuInven->WBP_ItemDetail->
			// SetRenderTranslation(ScalePosition);
		}
	}
	return Super::NativeOnMouseButtonDown(MyGeometry, MouseEvent);
}

void USlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                 UDragDropOperation*& OutOperation)
{
	UDragDropOperation* DragOp = NewObject<UDragDropOperation>();
	UItemObject* ItemObject = NewObject<UItemObject>();

    USlotEmpty* SlotEmpty = CreateWidget<USlotEmpty>(GetWorld(),SlotEmptyFactory);
	if (SlotEmpty)
	{
		SlotEmpty->SlotImage->SetBrush(SlotImage->GetBrush());
	}
	FSlateBrush Brush;
	Brush.SetResourceObject(nullptr);
	Brush.DrawAs = ESlateBrushDrawType::Type::NoDrawType;
	SlotImage->SetBrush(Brush);
	
    ItemObject->ItemStruct = ItemStruct;
	ItemStruct = FItemStruct();
	SlotCountUpdate(ItemStruct.ItemNum);
	
	ItemObject->SlotUi = this;
	
	DragOp->DefaultDragVisual = SlotEmpty;
	DragOp->Payload = ItemObject;
	DragOp->Pivot = EDragPivot::MouseDown;

	UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnMouseDrag"));
	OutOperation = DragOp;
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

bool USlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
	Swap(ItemStruct, ItemObject->ItemStruct);

	SlotImageUpdate();
	SlotCountUpdate(ItemStruct.ItemNum);
	
	UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnDrop"));
	return true;
	// return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void USlot::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	
	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
	ItemStruct = ItemObject->ItemStruct;
	SlotImageUpdate();
	UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnDrop 다른창에 했음"));
}
