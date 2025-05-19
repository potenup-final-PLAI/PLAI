// Fill out your copyright notice in the Description page of Project Settings.


#include "UiWorldMap.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/SizeBox.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"

UUiWorldMap::UUiWorldMap(const FObjectInitializer& FOI)
	:Super(FOI)
{
	bIsFocusable = true;
}

void UUiWorldMap::SetPlayerMinmapVector(FVector PlayerLocation)
{
	float U = (PlayerLocation.X - WorldMinFevtor.X) / (WorldMaxFevtor.X - WorldMinFevtor.X);
	float V = (PlayerLocation.Y - WorldMinFevtor.Y) / (WorldMaxFevtor.Y - WorldMinFevtor.Y);

	U = FMath::Clamp(U, 0.f, 1.f);
	V = FMath::Clamp(V, 0.f, 1.f);

	FVector2D PixelPos = FVector2D(U * 250, V * 250);
	
	if (auto* CanvasSlot = Cast<UCanvasPanelSlot>(MiniMapOverlay->Slot))
	{ CanvasSlot->SetPosition(PixelPos); }
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("UiWorldMap::SetPlayer MinmapVector Error"));
	}
}

void UUiWorldMap::NativeConstruct()
{
	Super::NativeConstruct();
	
	MaterialMapDynamic = UMaterialInstanceDynamic::Create(MaterialMapInterface,this);

	MiniMap->SetBrushFromMaterial(MaterialMapDynamic);
}

FReply UUiWorldMap::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InGeometry.IsUnderLocation(InMouseEvent.GetScreenSpacePosition()))
	{
		float Delta = -InMouseEvent.GetWheelDelta();  // +1 or -1
		CurrentZoom = FMath::Clamp(CurrentZoom + Delta * 0.1, 0, 2);

		if (MaterialMapDynamic)
		{
			MaterialMapDynamic->SetScalarParameterValue(TEXT("ZoomFactor"), CurrentZoom);
		}
		return FReply::Handled();
	}
	return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
}


void UUiWorldMap::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		SetPlayerMinmapVector(TestPlayer->GetActorLocation());
		// 2-2) 플레이어 Yaw(헤딩) 가져오기
		float Yaw = TestPlayer->GetActorRotation().Yaw;
		// (필요하면 -Yaw 로 반대로 돌리기도 함)
		// 2-3) 각도 적용
		PlayerRot->SetRenderTransformPivot(FVector2D(0.5f, 0.f));
		PlayerRot->SetRenderTransformAngle(Yaw - 90);

		float U = (TestPlayer->GetActorLocation().X - WorldMinFevtor.X) / (WorldMaxFevtor.X - WorldMinFevtor.X);
		float V = (TestPlayer->GetActorLocation().Y - WorldMinFevtor.Y) / (WorldMaxFevtor.Y - WorldMinFevtor.Y);
		
		U = FMath::Clamp(U, 0.f, 1.f);
		V = FMath::Clamp(V, 0.f, 1.f);
		
		// 2-3) CenterOffset에 해당 UV값 전달
		//    (Material에서 CenterOffset은 VectorParameter)
		MaterialMapDynamic->SetVectorParameterValue(TEXT("CenterOffset"),FLinearColor(U, V, 0.f, 0.f));
	}
}


// if (MiniMapSizeBox)
// {
// 	FGeometry ChildGeo = MiniMap->GetCachedGeometry();
// 	FVector2D ChildSize = ChildGeo.GetLocalSize();
// 	UE_LOG(LogTemp, Log, TEXT("MiniMapSizeBox 크기: %s"), *ChildSize.ToString());
// }
