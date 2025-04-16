// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PLAI/Item/Item/ItemStruct.h"
#include "Slot.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API USlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemStruct ItemStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemMeshStructIndex ItemMeshStructIndex;

	UPROPERTY(EditAnywhere)
	class UTexture2D* Texture;

	UPROPERTY(EditAnywhere)
	class AItem* Item;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* SlotImage;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* SlotCount;

	void SlotCountUpdate(const int32 Count);

	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
