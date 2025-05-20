// Fill out your copyright notice in the Description page of Project Settings.


#include "UiWorldMap.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "PLAI/Item/UI/Inventory/Map/UiWorldPlayerIcon.h"
#include "Components/SizeBox.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"

UUiWorldMap::UUiWorldMap(const FObjectInitializer& FOI)
	:Super(FOI)
{
	bIsFocusable = true;
}

void UUiWorldMap::SetRefreshPlayerList()
{
	TestPlayers.Empty();
	MiniMapCanvasIcon->GetAllChildren().Empty();
	
	if (AGameStateBase* GS = GetWorld()->GetGameState())
	{
		for (APlayerState* PS : GS->PlayerArray)
		{
			if (ATestPlayer* TP = Cast<ATestPlayer>(PS->GetPawn()))
			{
				UIWorldPlayerIcon = CreateWidget<UUiWorldPlayerIcon>(GetWorld(),UiWorldPlayerIconFactory);
				MiniMapCanvasIcon->AddChild(UIWorldPlayerIcon);
				if (UCanvasPanelSlot* Icon = Cast<UCanvasPanelSlot>(UIWorldPlayerIcon))
				{
					Icon->SetSize(FVector2d(30,30));
				}
				TestPlayers.Add(TP);
				UE_LOG(LogTemp, Warning, TEXT("UIWorldMap::SetRefreshPlayerList() 플레이어 갯수[%i]"),TestPlayers.Num());
			}
		}
	}
}

void UUiWorldMap::SetPlayerIconMinimap()
{
	if (TestPlayers.Num() == 0 || MiniMapCanvasIcon->GetAllChildren().Num() == 0){UE_LOG(LogTemp,Warning,TEXT("UiWorldMap 플레이어 미니맵아이콘 없음")) return;}
	for (int i = 0; i < TestPlayers.Num(); i++)
	{
		float U = (TestPlayers[i]->GetActorLocation().X - WorldMinFevtor.X) / (WorldMaxFevtor.X - WorldMinFevtor.X);
		float V = (TestPlayers[i]->GetActorLocation().Y - WorldMinFevtor.Y) / (WorldMaxFevtor.Y - WorldMinFevtor.Y);

		U = FMath::Clamp(U, 0.f, 1.f);
		V = FMath::Clamp(V, 0.f, 1.f);

		FVector2D PixelPos = FVector2D(U * MiniMapSize.X, V * MiniMapSize.Y);
	
		if (auto* CanvasSlot = Cast<UCanvasPanelSlot>(MiniMapCanvasIcon->GetChildAt(i)->Slot))
		{
			CanvasSlot->SetPosition(PixelPos);
		}
		else
		{ UE_LOG(LogTemp,Warning,TEXT("UiWorldMap::SetPlayer MinmapVector Error")); }
	}
}

void UUiWorldMap::ExtendMap()
{
	if (auto* CanvasSlot = Cast<UCanvasPanelSlot>(MiniMapCanvas->Slot))
	{
		// 1) 최초 호출 시 원래 크기/위치 저장
		if (bExtendMap == false)
		{
			CanvasSlot->SetSize(MiniMapSizeL);
			CanvasSlot->SetAnchors(FAnchors(0.5,0.5));
			CanvasSlot->SetPosition(FVector2D::ZeroVector);
			CanvasSlot->SetAlignment(FVector2D(0.5,0.5));
			MiniMapSize = MiniMapSizeL;
			bExtendMap = true;
		}
		else
		{
			CanvasSlot->SetSize(MiniMapSizeS);
			CanvasSlot->SetAnchors(FAnchors(1,1));
			CanvasSlot->SetPosition(FVector2D(-25,-25));
			CanvasSlot->SetAlignment(FVector2D(1,1));
			MiniMapSize = MiniMapSizeS;;
			bExtendMap = false;
		}
	}
}

void UUiWorldMap::SetPlayerMinmapVector(FVector PlayerLocation)
{
	float U = (PlayerLocation.X - WorldMinFevtor.X) / (WorldMaxFevtor.X - WorldMinFevtor.X);
	float V = (PlayerLocation.Y - WorldMinFevtor.Y) / (WorldMaxFevtor.Y - WorldMinFevtor.Y);

	U = FMath::Clamp(U, 0.f, 1.f);
	V = FMath::Clamp(V, 0.f, 1.f);

	FVector2D PixelPos = FVector2D(U * MiniMapSize.X, V * MiniMapSize.Y);
	
	if (auto* CanvasSlot = Cast<UCanvasPanelSlot>(MiniMapOverlay->Slot))
	{ CanvasSlot->SetPosition(PixelPos); }
	else
	{ UE_LOG(LogTemp,Warning,TEXT("UiWorldMap::SetPlayer MinmapVector Error")); }
}

void UUiWorldMap::NativeConstruct()
{
	Super::NativeConstruct();
	
	MaterialMapDynamic = UMaterialInstanceDynamic::Create(MaterialMapInterface,this);
	MiniMap->SetBrushFromMaterial(MaterialMapDynamic);

	SetRefreshPlayerList();
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

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->WasInputKeyJustPressed(EKeys::M))
		{
			ExtendMap();
		}
	}
	if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		SetPlayerIconMinimap();
		// SetPlayerMinmapVector(TestPlayer->GetActorLocation());
		
		float Yaw = TestPlayer->GetActorRotation().Yaw;
		
		PlayerRot->SetRenderTransformPivot(FVector2D(0.5f, 0.f));
		PlayerRot->SetRenderTransformAngle(Yaw - 90);
	
		float U = (TestPlayer->GetActorLocation().X - WorldMinFevtor.X) / (WorldMaxFevtor.X - WorldMinFevtor.X);
		float V = (TestPlayer->GetActorLocation().Y - WorldMinFevtor.Y) / (WorldMaxFevtor.Y - WorldMinFevtor.Y);
		
		U = FMath::Clamp(U, 0.f, 1.f);
		V = FMath::Clamp(V, 0.f, 1.f);
		
		MaterialMapDynamic->SetVectorParameterValue(TEXT("CenterOffset"),FLinearColor(U, V, 0.f, 0.f));
	}
}


// if (MiniMapSizeBox)
// {
// 	FGeometry ChildGeo = MiniMap->GetCachedGeometry();
// 	FVector2D ChildSize = ChildGeo.GetLocalSize();
// 	UE_LOG(LogTemp, Log, TEXT("MiniMapSizeBox 크기: %s"), *ChildSize.ToString());
// }
