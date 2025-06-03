// Fill out your copyright notice in the Description page of Project Settings.


#include "UiWorldMap.h"

#include "QuestOrderActor.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "PLAI/Item/UI/Inventory/Map/UiWorldPlayerIcon.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "PLAI/Item/GameInstance/WorldGi.h"
#include "PLAI/Item/GameState/GameStateOpen.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/Monster/MonWorld/MonWorld.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/TestPlayer/InputComp/InputComp.h"
#include "PLAI/Item/UI/Inventory/Quest/UiQuest.h"

void UUiWorldMap::NextQuestType(EQuestType Quest)
{
	if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		if (!TestPlayer->IsLocallyControlled()) return;
	}
	// if (QuestType != EQuestType::E_MonsterTurn)
	// {
	// 	QuestType = static_cast<EQuestType>(static_cast<int32>(QuestType) + 1);
	// 	UE_LOG(LogTemp,Warning,TEXT("UiWorldMap 현재 QuestType 이름? [%s]"),*UEnum::GetValueAsString(QuestType))
	// }
	// else
	// {
	// 	QuestType = static_cast<EQuestType>(0);
	// }
	QuestType = Quest;
	
	switch (QuestType)
	{
	case EQuestType::A_GetEquip:
		NextQuestMinimap(EQuestType::A_GetEquip);
		MenuInven->Wbp_UiQuest->NextQuest(1,FString(TEXT("전설의 검을 뽑다")),
			FString(TEXT("[ M ] 키를 눌러 전설의 검 위치를 확인후 검을 뽑아 장비를 획득하고 "
				"[ E ] 와 ""[ S ] 키를 눌러 장비를 확인하세요")));
		break;
		
	case EQuestType::B_NetNpcHearty:
		NextQuestMinimap(EQuestType::B_NetNpcHearty);
		MenuInven->Wbp_UiQuest->NextQuest(2,FString(TEXT("마을 사람들과 친해지기 1")),
			FString(TEXT("마을의 활기찬 자경단 Npc에게 [파란디온]에 대해 물어봐주세요")));
		break;

	case EQuestType::C_NetNpcScared:
		NextQuestMinimap(EQuestType::C_NetNpcScared);
		MenuInven->Wbp_UiQuest->NextQuest(3,FString(TEXT("마을 사람들과 친해지기 2")),
			FString(TEXT("겁에질린 마을 Npc에게 [파란디온] 에대해 물어봐 주세요")));
		break;

	case EQuestType::D_Store:
		NextQuestMinimap(EQuestType::D_Store);
		MenuInven->Wbp_UiQuest->NextQuest(4,FString(TEXT("마을 상점 이용하기")),
			FString(TEXT("상점 Npc를 클릭한후 [ I ]키를 눌러 인벤토리와 구매가능 아이템을 구매해보세요")));
		break;

	case EQuestType::E_MonsterTurn:
		NextQuestMinimap(EQuestType::E_MonsterTurn);
		MenuInven->Wbp_UiQuest->NextQuest(5,FString(TEXT("마을을 위협하는 몬스터를 처치하자")),
			FString(TEXT("마을 밖의 무시무시한 몬스터를 처치하여 전리품을 획득하세요")));
		break;

	case EQuestType::F_Store:
		NextQuestMinimap(EQuestType::D_Store);
		MenuInven->Wbp_UiQuest->NextQuest(6,FString(TEXT("상점에서 장비를 사고 착용해보자")),
			FString(TEXT("획득한 전리품을 팔고 골드를 이용해서 전설장비 무기,망토 장비를 사보고 [ E ], [ I ] 키를눌러 착용해보자 ")));
		break;
	} 
}

void UUiWorldMap::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	SetPlayerIconMinimap();
}

void UUiWorldMap::NativeConstruct()
{
	Super::NativeConstruct();
	
	GameStateOpen = Cast<AGameStateOpen>(GetWorld()->GetGameState());
	GameStateOpen->FindMiniMapGuideIcon();
	
	MaterialMapDynamic = UMaterialInstanceDynamic::Create(MaterialMapInterface,this);
	MiniMap->SetBrushFromMaterial(MaterialMapDynamic);

    SetRefreshPlayerList();
	
	if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{ TestPlayer->InputComp->OnInputMap.BindUObject(this, &UUiWorldMap::ExtendMap); }
	
	MaterialMapDynamic->SetScalarParameterValue(TEXT("ZoomFactor"), 0.5);
	
	if (UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
		{NextQuestType(EQuestType::A_GetEquip);},1.0f,false);
		
		UE_LOG(LogTemp,Warning,TEXT("UiWolrdMap WorldGi 있다"))
		if (WorldGi->bFirstQuest == false)
		{
			UE_LOG(LogTemp,Warning,TEXT("UiWolrdMap WorldGi bFirstQuest == false"))
			WorldGi->bFirstQuest = true;
		}
	}
}

void UUiWorldMap::SetRefreshPlayerList()
{
	TestPlayers.Empty();
	MiniMapCanvasIcon->GetAllChildren().Empty();
	
	for (APlayerState* PS : GameStateOpen->PlayerArray)
	{
		if (ATestPlayer* TP = Cast<ATestPlayer>(PS->GetPawn()))
		{
			// if (!TP -> IsLocallyControlled())return;
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

	QuestActors.Empty();
	MiniMapCanvasIconQuest->GetAllChildren().Empty();

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),AQuestOrderActor::StaticClass(),Actors);
	for (int i = 0; i < Actors.Num(); i++)
	{
		if (AQuestOrderActor* QuestActor = Cast<AQuestOrderActor>(Actors[i]))
		{
			QuestActors.Add(QuestActor);
		}
	}

	for (int i = 0; i < QuestActors.Num(); i++)
	{
	    UIWorldMapGuideIcon = CreateWidget<UUIWorldMapGuide>(GetWorld(),UIWorldMapGuideFactory);
		if (UIWorldMapGuideIcon)
		{
			MiniMapCanvasIconQuest->AddChild(UIWorldMapGuideIcon);
		}
		if (UCanvasPanelSlot* Icon = Cast<UCanvasPanelSlot>(UIWorldMapGuideIcon->Slot))
		{
			Icon->SetAlignment(FVector2D(0.5,0.5));
			Icon->SetSize(FVector2d(60,60));
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
		float U = (TestPlayers[i]->GetActorLocation().X -
			WorldMinFevtor.X) / (WorldMaxFevtor.X - WorldMinFevtor.X);
		float V = (TestPlayers[i]->GetActorLocation().Y -
			WorldMinFevtor.Y) / (WorldMaxFevtor.Y - WorldMinFevtor.Y);
		
		U = FMath::Clamp(U, 0.f, 1.f);
		V = FMath::Clamp(V, 0.f, 1.f);

		FVector2D PixelPos = FVector2D(U * MiniMapSize.X, V * MiniMapSize.Y);
		float Yaw = TestPlayers[i]->GetActorRotation().Yaw;
	
		if (auto* CanvasSlot = Cast<UCanvasPanelSlot>(MiniMapCanvasIcon->GetChildAt(i)->Slot))
		{
			if (TestPlayers[i]->InvenComp && TestPlayers[i]->InvenComp->MenuInven && TestPlayers[i]->InvenComp->MenuInven->Wbp_UiWorldMap &&
				TestPlayers[i]->InvenComp->MenuInven->Wbp_UiWorldMap == this)
			{
				CanvasSlot->SetPosition(FVector2d(125,125));
				MiniMapCanvasIcon->GetChildAt(i)->SetRenderTransformAngle(Yaw - 90);
				MaterialMapDynamic->SetVectorParameterValue(TEXT("CenterOffset"),FLinearColor(U, V, 0.f, 0.f));
			}
			
			if (bExtendMap == true)
			{
				CanvasSlot->SetPosition(PixelPos);
			}
		}
		else { UE_LOG(LogTemp,Warning,TEXT("UiWorldMap::SetPlayer MinmapVector Error")); }
	}
	
	for (int i = 0; i < QuestActors.Num(); i++)
	{
		FVector Location = QuestActors[i]->GetActorLocation();
		
		float U = (Location.X - WorldMinFevtor.X) / (WorldMaxFevtor.X - WorldMinFevtor.X);
		float V = (Location.Y - WorldMinFevtor.Y) / (WorldMaxFevtor.Y - WorldMinFevtor.Y);

		U = FMath::Clamp(U, 0.f, 1.f);
		V = FMath::Clamp(V, 0.f, 1.f);
		
		FVector2D PixelPos = FVector2D(U * MiniMapSize.X, V * MiniMapSize.Y);

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(MiniMapCanvasIconQuest->GetChildAt(i)->Slot))
		{
			if (bExtendMap == true)
			{
				CanvasSlot->SetPosition(PixelPos);
			}
		}
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
			MaterialMapDynamic->SetScalarParameterValue(TEXT("ZoomFactor"), 0.4);
			CanvasSlot->SetSize(MiniMapSizeS);
			CanvasSlot->SetAnchors(FAnchors(1,1));
			CanvasSlot->SetPosition(FVector2D(-10,-10));
			CanvasSlot->SetAlignment(FVector2D(1,1));
			MiniMapSize = MiniMapSizeS;;
			bExtendMap = false;
		}
	}
}


void UUiWorldMap::NextQuestMinimap(EQuestType Quest)
{
	UE_LOG(LogTemp,Warning,TEXT("UiWorldMap NextQuest [%s]"),*UEnum::GetValueAsString(Quest))
	for (int i = 0; i < QuestActors.Num(); i++)
	{
		if (UUIWorldMapGuide* Icon = Cast<UUIWorldMapGuide>(MiniMapCanvasIconQuest->GetChildAt(i)))
		{
			if (!IsValid(Icon)) return;
			if (QuestActors[i]->QuestType == Quest)
			{
				if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
				{
					TestPlayer->QuestLocation = QuestActors[i]->GetActorLocation();
				}
				Icon->SetRenderScale(FVector2D(1.2f));
				Icon->SetRenderOpacity(1);

				if (!IsValid(Icon)) return;

				
				
				GetWorld()->GetTimerManager().SetTimer(Icon->TimerHandle,FTimerDelegate::CreateWeakLambda(this, [this,Icon]()
				{
					Icon->CurrentTime += GetWorld()->GetDeltaSeconds();
					Icon->SetRenderScale(FVector2D(0.5 + Icon->CurrentTime * 0.5));
					Icon->SetRenderOpacity(0.5 + Icon->CurrentTime*0.25);
					if (Icon->CurrentTime > 2)
					{
						Icon->CurrentTime = 0;
					}
				}),0.02f,true);
			}
			else
			{
				GetWorld()->GetTimerManager().ClearTimer(Icon->TimerHandle);
				Icon->SetRenderScale(FVector2D(0.5));
				Icon->SetRenderOpacity(0.5);
			}
		}
	}
	
	// if (QuestType != EQuestType::E_MonsterTurn)
	// {
	// 	QuestType = static_cast<EQuestType>(static_cast<int32>(QuestType) + 1);
	// 	UE_LOG(LogTemp,Warning,TEXT("UiWorldMap 현재 QuestType 이름? [%s]"),*UEnum::GetValueAsString(QuestType))
	// }
	// else
	// {
	// 	QuestType = static_cast<EQuestType>(0);
	// }
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