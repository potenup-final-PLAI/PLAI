// Fill out your copyright notice in the Description page of Project Settings.


#include "UiWorldMap.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"

void UUiWorldMap::SetPlayerMinmapVector(FVector PlayerLocation)
{
	float U = (PlayerLocation.X - WorldMinFevtor.X) / (WorldMaxFevtor.X - WorldMinFevtor.X);
	float V = (PlayerLocation.Y - WorldMinFevtor.Y) / (WorldMaxFevtor.Y - WorldMinFevtor.Y);

	U = FMath::Clamp(U, 0.f, 1.f);
	V = FMath::Clamp(V, 0.f, 1.f);

	FVector2D PixelPos = FVector2D(U * MiniMapSize.X, V * MiniMapSize.Y);

	PlayerIcon->SetRenderTranslation(PixelPos);
	
	// if (UCanvasPanelSlot* IconSlot = Cast<UCanvasPanelSlot>(PlayerIcon->Slot))
	// {
	// 	IconSlot->SetPosition(FVector2d(PixelPos));
	// 	UE_LOG(LogTemp,Warning,TEXT("%s"),TEXT("UiWorldMap UCanvasPanelSlot PlayerIcon CanvasSlot있음"));
	// }
	// else
	// {
	// 	UE_LOG(LogTemp,Warning,TEXT("%s"),TEXT("UiWorldMap UCanvasPanelSlot PlayerIcon CanvasSlot으로 안됨"));
	// }
}

void UUiWorldMap::NativeConstruct()
{
	Super::NativeConstruct();
	
	MaterialMapDynamic = UMaterialInstanceDynamic::Create(MaterialMapInterface,this);

	MiniMap->SetBrushFromMaterial(MaterialMapDynamic);

	MaterialMapDynamic->SetScalarParameterValue("ZoomFactor", 1.0f);
	MaterialMapDynamic->SetVectorParameterValue("CenterOffset", FVector4(0.5f,0.5f,0,0));
	
}

void UUiWorldMap::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		SetPlayerMinmapVector(TestPlayer->GetActorLocation());
	}
}



// if (MiniMapSizeBox)
// {
// 	FGeometry ChildGeo = MiniMap->GetCachedGeometry();
// 	FVector2D ChildSize = ChildGeo.GetLocalSize();
// 	UE_LOG(LogTemp, Log, TEXT("MiniMapSizeBox 크기: %s"), *ChildSize.ToString());
// }
