// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SlotEmpty.h"
#include "Blueprint/UserWidget.h"
#include "PLAI/Item/Item/Item.h"
#include "PLAI/Item/Item/ItemStruct.h"
#include "PLAI/Item/UI/Inventory/ItemDetail/ItemDetail.h"
#include "Slot.generated.h"

UENUM(BlueprintType)
enum class ESLotType : uint8
{
	Slot,
	Equip,
	Store,
	Inven,
	Creature,
};

/**
 * 
 */
UCLASS()
class PLAI_API USlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere)
	ESLotType SlotType = ESLotType::Slot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemStruct ItemStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemStructTable ItemStructTable;
	 
	UPROPERTY(EditAnywhere)
	class UTexture2D* Texture;
	
    UPROPERTY(EditAnywhere)
	TSubclassOf<AItem> ItemFactory;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<USlotEmpty> SlotEmptyFactory;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UItemDetail> ItemDetailFactory;
	
	UPROPERTY(EditAnywhere)
	class UItemDetail* ItemDetail;
	
	UPROPERTY(EditAnywhere)
	class AItem* ParentItem;

	UPROPERTY(EditAnywhere)
	bool bItemDetail = true;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* SlotImage;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* SlotCount;

    UPROPERTY(EditAnywhere)
	class UEquipInven* EquipInven;
	
	void SlotCountUpdate(const int32 Count);

	void SlotImageUpdate();

    virtual void NativeConstruct() override;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UDataTable* ItemTable;

	FItemStructTable* ItemTableFind();
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
};
