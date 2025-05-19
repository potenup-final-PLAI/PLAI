// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UiWorldMap.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUiWorldMap : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	UMaterialInterface* MaterialMapInterface;
	
	UPROPERTY(EditAnywhere)
	UMaterialInstanceDynamic* MaterialMapDynamic;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* MiniMapCanvas;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* MiniMap;

	UPROPERTY(meta = (BindWidget))
	class UImage* PlayerIcon;

	FVector2D WorldMinFevtor = FVector2D(-22700.0, -21260.0);
	FVector2D WorldMaxFevtor = FVector2D(27700.0, 29140.0);

	FVector2D MiniMapSize = FVector2D(250.0,250.0);

	void SetPlayerMinmapVector(FVector PlayerLocation);

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
