// Fill out your copyright notice in the Description page of Project Settings.


#include "UiWorldMap.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "PLAI/Item/UI/Inventory/Map/UiWorldPlayerIcon.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/TestPlayer/InputComp/InputComp.h"

// UUiWorldMap::UUiWorldMap(const FObjectInitializer& FOI)
// 	:Super(FOI)
// {
// 	bIsFocusable = true;
// }


void UUiWorldMap::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	SetPlayerIconMinimap();
}

void UUiWorldMap::NativeConstruct()
{
	Super::NativeConstruct();
	
	MaterialMapDynamic = UMaterialInstanceDynamic::Create(MaterialMapInterface,this);
	MiniMap->SetBrushFromMaterial(MaterialMapDynamic);

    SetRefreshPlayerList();
	if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{ TestPlayer->InputComp->OnInputMap.BindUObject(this, &UUiWorldMap::ExtendMap); }
	
	MaterialMapDynamic->SetScalarParameterValue(TEXT("ZoomFactor"), 0.5);
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
				if (!TP -> IsLocallyControlled())return;
				UIWorldPlayerIcon = CreateWidget<UUiWorldPlayerIcon>(GetWorld(),UiWorldPlayerIconFactory);

				MiniMapCanvasIcon->AddChild(UIWorldPlayerIcon);
				TestPlayers.Add(TP);
				if (UCanvasPanelSlot* Icon = Cast<UCanvasPanelSlot>(UIWorldPlayerIcon->Slot))
				{
					Icon->SetAlignment(FVector2D(0.5,0.5));
					Icon->SetSize(FVector2d(60,60));
				}
				UE_LOG(LogTemp, Warning, TEXT("UIWorldMap::SetRefreshPlayerList() 플레이어 갯수[%i]"),TestPlayers.Num());
			}
		}
	}
}

void UUiWorldMap::SetPlayerIconMinimap()
{
	for (int i = 0; i < TestPlayers.Num(); i++)
	{
		if (!TestPlayers[i])
		{
			SetRefreshPlayerList();
			UE_LOG(LogTemp,Warning,TEXT("UiWorldMap SetRefresh하고 한번더 체크 플레이어 갯수 [%d] 없냐?"),TestPlayers.Num()) return;
		}
		float U = (TestPlayers[i]->GetActorLocation().X - WorldMinFevtor.X) / (WorldMaxFevtor.X - WorldMinFevtor.X);
		float V = (TestPlayers[i]->GetActorLocation().Y - WorldMinFevtor.Y) / (WorldMaxFevtor.Y - WorldMinFevtor.Y);
		
		U = FMath::Clamp(U, 0.f, 1.f);
		V = FMath::Clamp(V, 0.f, 1.f);

		FVector2D PixelPos = FVector2D(U * MiniMapSize.X, V * MiniMapSize.Y);
		float Yaw = TestPlayers[i]->GetActorRotation().Yaw;
	
		if (auto* CanvasSlot = Cast<UCanvasPanelSlot>(MiniMapCanvasIcon->GetChildAt(i)->Slot))
		{
			MiniMapCanvasIcon->GetChildAt(i)->SetRenderTransformAngle(Yaw - 90);
			MaterialMapDynamic->SetVectorParameterValue(TEXT("CenterOffset"),FLinearColor(U, V, 0.f, 0.f));

			if (bExtendMap == true)
			{ CanvasSlot->SetPosition(PixelPos); }
			else
			{ CanvasSlot->SetPosition(FVector2d(125,125)); }
		}
		else { UE_LOG(LogTemp,Warning,TEXT("UiWorldMap::SetPlayer MinmapVector Error")); }
	}
}

void UUiWorldMap::ExtendMap()
 {
	SetRefreshPlayerList();
	
	if (auto* CanvasSlot = Cast<UCanvasPanelSlot>(MenuInven->Wbp_UiWorldMap->Slot))
	{
		UE_LOG(LogTemp,Warning,TEXT("UUiWorldMap::NativeConstruct 미니맵사이즈 [%s]"),*CanvasSlot->GetSize().ToString());
		if (bExtendMap == false)
		{
			CanvasSlot->SetSize(MiniMapSizeL);
			MaterialMapDynamic->SetScalarParameterValue(TEXT("ZoomFactor"), 1);
			CanvasSlot->SetAnchors(FAnchors(0.5,0.5));
			CanvasSlot->SetPosition(FVector2D::ZeroVector);
			CanvasSlot->SetAlignment(FVector2D(0.5,0.5));
			MiniMapSize = MiniMapSizeL;
			bExtendMap = true;
		}
		else
		{
			MaterialMapDynamic->SetScalarParameterValue(TEXT("ZoomFactor"), 0.5);
			CanvasSlot->SetSize(MiniMapSizeS);
			CanvasSlot->SetAnchors(FAnchors(1,1));
			CanvasSlot->SetPosition(FVector2D(-10,-10));
			// CanvasSlot->SetPosition(FVector2D(0,0));
			CanvasSlot->SetAlignment(FVector2D(1,1));
			MiniMapSize = MiniMapSizeS;;
			bExtendMap = false;
		}
	}
}

FReply UUiWorldMap::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InGeometry.IsUnderLocation(InMouseEvent.GetScreenSpacePosition()))
	{
		float Delta = -InMouseEvent.GetWheelDelta();  // +1 or -1
		CurrentZoom = FMath::Clamp(CurrentZoom + Delta * 0.1, 0, 1);

		if (MaterialMapDynamic)
		{
			if (bExtendMap == false)
			{
				MaterialMapDynamic->SetScalarParameterValue(TEXT("ZoomFactor"), CurrentZoom);
			}
			else
			{
				MaterialMapDynamic->SetScalarParameterValue(TEXT("ZoomFactor"), 1);
			}
		}
		return FReply::Handled();
	}
	return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
}

// if (!TestPlayers[i]){UE_LOG(LogTemp,Warning,TEXT("UiWorldMap 플레이어 갯수 [%d] 없냐?"),TestPlayers.Num()) return;}

// if (MiniMapSizeBox)
// {
// 	FGeometry ChildGeo = MiniMap->GetCachedGeometry();
// 	FVector2D ChildSize = ChildGeo.GetLocalSize();
// 	UE_LOG(LogTemp, Log, TEXT("MiniMapSizeBox 크기: %s"), *ChildSize.ToString());
// }


// void UUiWorldMap::SetPlayerMinmapVector(FVector PlayerLocation)
// {
// 	float U = (PlayerLocation.X - WorldMinFevtor.X) / (WorldMaxFevtor.X - WorldMinFevtor.X);
// 	float V = (PlayerLocation.Y - WorldMinFevtor.Y) / (WorldMaxFevtor.Y - WorldMinFevtor.Y);
//
// 	U = FMath::Clamp(U, 0.f, 1.f);
// 	V = FMath::Clamp(V, 0.f, 1.f);
//
// 	FVector2D PixelPos = FVector2D(U * MiniMapSize.X, V * MiniMapSize.Y);
// 	
// 	if (auto* CanvasSlot = Cast<UCanvasPanelSlot>(MiniMapOverlay->Slot))
// 	{ CanvasSlot->SetPosition(PixelPos); }
// 	else
// 	{ UE_LOG(LogTemp,Warning,TEXT("UiWorldMap::SetPlayer MinmapVector Error")); }
// }

// if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
// {
// 	if (!TestPlayer){UE_LOG(LogTemp,Warning,TEXT("UiWorldMap 플레이어 없음")) return;}
// 	float Yaw = TestPlayer->GetActorRotation().Yaw;
// 	
// 	PlayerRot->SetRenderTransformPivot(FVector2D(0.5f, 0.f));
// 	PlayerRot->SetRenderTransformAngle(Yaw - 90);
//
// 	float U = (TestPlayer->GetActorLocation().X - WorldMinFevtor.X) / (WorldMaxFevtor.X - WorldMinFevtor.X);
// 	float V = (TestPlayer->GetActorLocation().Y - WorldMinFevtor.Y) / (WorldMaxFevtor.Y - WorldMinFevtor.Y);
// 	
// 	U = FMath::Clamp(U, 0.f, 1.f);
// 	V = FMath::Clamp(V, 0.f, 1.f);
// 	
// 	MaterialMapDynamic->SetVectorParameterValue(TEXT("CenterOffset"),FLinearColor(U, V, 0.f, 0.f));
// }

// SetPlayerMinmapVector(TestPlayer->GetActorLocation());