// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UiWorldPlayerIcon.h"
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
	// UUiWorldMap(const FObjectInitializer& FOI);

	UPROPERTY(EditAnywhere)
	UMaterialInterface* MaterialMapInterface;
	
	UPROPERTY(EditAnywhere)
	UMaterialInstanceDynamic* MaterialMapDynamic;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* MiniMapCanvas;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* MiniMapCanvasIcon;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* MiniMap;

	UPROPERTY(EditAnywhere)
	class UMenuInven* MenuInven;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUiWorldPlayerIcon>UiWorldPlayerIconFactory;

	UPROPERTY(EditAnywhere)
	class UUiWorldPlayerIcon* UIWorldPlayerIcon;

	UPROPERTY(EditAnywhere)
	TArray<class ATestPlayer*>TestPlayers;
	
	void SetRefreshPlayerList();
	
	void SetPlayerIconMinimap();
	
	UPROPERTY(EditAnywhere)
	FVector2D WorldMinFevtor = FVector2D(-22700.0, -21260.0);
	UPROPERTY(EditAnywhere)
	FVector2D WorldMaxFevtor = FVector2D(27700.0, 29140.0);
	UPROPERTY(EditAnywhere)
	FVector2D MiniMapSize = FVector2D(250.0,250.0);
	UPROPERTY(EditAnywhere)
	FVector2D MiniMapSizeS = FVector2D(250.0,250.0);
	UPROPERTY(EditAnywhere)
	FVector2D MiniMapSizeL = FVector2D(750.0,750.0);
	
	UPROPERTY(EditAnywhere)
	FVector2D OrigPosition;
	UPROPERTY(EditAnywhere)
	bool      bExtendMap = false;
	void ExtendMap();

	UPROPERTY(EditAnywhere)
	float CurrentZoom = 0.5f; 

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};


// UPROPERTY(meta = (BindWidget))
// class UImage* PlayerIcon;
// UPROPERTY(meta = (BindWidget))
// class UImage* PlayerRot;
// UPROPERTY(meta = (BindWidget))
// class UOverlay* MiniMapOverlay;