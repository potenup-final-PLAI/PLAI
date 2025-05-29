// Fill out your copyright notice in the Description page of Project Settings.


#include "Slot.h"

#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PLAI/Item/Item/ItemObject.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/Login/LoginComp.h"
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
}

void USlot::NativeConstruct()
{
	Super::NativeConstruct();
	ParentItem = ItemFactory->GetDefaultObject<AItem>();

	FSlateBrush Brush;
	Brush.SetResourceObject(nullptr);
	Brush.DrawAs = ESlateBrushDrawType::NoDrawType;
	Brush.TintColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.15f);
	SlotImage->SetBrush(Brush);
}

FItemStructTable* USlot::ItemTableFind()
{
	const TArray<FName> Rownames = ItemTable->GetRowNames();

	for (const FName& RowName : Rownames)
	{
		FItemStructTable* Row = ItemTable->FindRow<FItemStructTable>(RowName, TEXT("ItemTable Slot"));
		if (Row && Row->ItemIndex == ItemStructTable.ItemIndex
			&& Row->ItemTop == ItemStructTable.ItemTop
			&& Row->ItemIndexType == ItemStructTable.ItemIndexType
			&& Row->ItemIndexDetail == ItemStructTable.ItemIndexDetail)
		{
			return Row;
		}
	}
	return nullptr;
}

FReply USlot::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (ItemStructTable.ItemTop == -1)
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
			TestPlayer->InvenComp->MenuInven->WBP_ItemDetail->SetVisibility(bItemDetail ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
			TestPlayer->InvenComp->MenuInven->WBP_ItemDetail->SetItemDetail(*ItemTableFind());
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
	
    ItemObject->ItemStructTable = ItemStructTable;
	ItemStructTable = FItemStructTable();
	SlotCountUpdate(ItemStructTable.ItemNum);
	
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

	if (ItemObject && ItemObject->bBuy)
	{
		if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
		{
			if (TestPlayer->LoginComp->UserFullInfo.inventory_info.gold >= ItemObject->ItemStructTable.ItemGold)
			{ TestPlayer->InvenComp->SetGold(-ItemObject->ItemStructTable.ItemGold); }
			else
			{
				UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnMouseDrag 아이템을 살려는데 돈이 없네"));
				return false;
			}
		}
	}
	
	// Swap 추가 나중에 네트워크 동기화는 확인안해봐서 확인해볼것
	if (ItemStructTable.ItemTop != -1)
	{
        if (USlot* SlotPre = Cast<USlot>(ItemObject->SlotUi))
        {
	        SlotPre->ItemStructTable = ItemStructTable;
        	ItemStructTable = ItemObject->ItemStructTable;
        	SlotPre->SlotImageUpdate();
        	SlotImageUpdate();
        }
		return true;
	}
	// 밑에거는 동기화까지는 끝내는거 확인했던 함수임!!
	else
	{
		// UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
		Swap(ItemStructTable, ItemObject->ItemStructTable);

		SlotImageUpdate();
		SlotCountUpdate(ItemStructTable.ItemNum);
	
		UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnDrop"));
		return true;
		// return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	}
}

void USlot::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	
	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
	ItemStructTable = ItemObject->ItemStructTable;
	SlotImageUpdate();
	UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnDrop 다른창에 했음"));
}

void USlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (ItemDetail == nullptr && ItemStructTable.ItemTop != -1)
	{
		FVector2d MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld()) + FVector2d(-100,100);
		ItemDetail = CreateWidget<UItemDetail>(GetWorld(),ItemDetailFactory);
		if (ItemDetail)
		{
			ItemDetail->AddToViewport();
			ItemDetail->SetItemDetail(ItemStructTable);
			ItemDetail->SetPositionInViewport(MousePos);
		}
	}
}

void USlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	if (ItemDetail)
	{
		ItemDetail->RemoveFromParent();
		ItemDetail=nullptr;
	}
}


// Swap(ItemObject->SlotUi->ItemStructTable, ItemStructTable);
// SlotImageUpdate();
// ItemObject->SlotUi->SlotImageUpdate();
// SlotCountUpdate(ItemStructTable.ItemNum);
// ItemObject->SlotUi->ItemStructTable = ItemStructTable;
// ItemObject->SlotUi->SlotImageUpdate();
// UE_LOG(LogTemp, Display, TEXT("Slot::NativeOnDrop"));