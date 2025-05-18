// Fill out your copyright notice in the Description page of Project Settings.

#include "UiPortal.h"

#include "BaseFile/PLAIPlayerController.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"
#include "PLAI/Item/Monster/Monster.h"
#include "PLAI/Item/Monster/MonSpawn/MonSpawn.h"
#include "PLAI/Item/Portal/Warp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"

void UUiPortal::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button_Village->OnClicked.AddDynamic(this,&UUiPortal::OnButton_Village);
	Button_Mountain->OnClicked.AddDynamic(this,&UUiPortal::OnButton_Mountain);
	Button_Dessert->OnClicked.AddDynamic(this,&UUiPortal::OnButton_Dessert);
	Button_Cave->OnClicked.AddDynamic(this,&UUiPortal::OnButton_Cave);
	Button_OpenMap->OnClicked.AddDynamic(this,&UUiPortal::OnButton_OpenMap);
}



void UUiPortal::WarpTestPlayer(EMonSpawnType SpawnType)
{
	TArray<AActor*> Mons;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonster::StaticClass(), Mons);

	for (AActor* Mon : Mons)
	{
		if (AMonster* Monster = Cast<AMonster>(Mon))
		{
			Monster->Destroy();
			break;
		}
	}
	
	UE_LOG(LogTemp,Warning,TEXT("UiPortal 어디소환중? [%s]"),*UEnum::GetValueAsString(SpawnType))
	
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWarp::StaticClass(), Actors);
	
	if (Actors.Num() == 0){UE_LOG(LogTemp,Warning,TEXT("UiPortal 없냐? %d"),Actors.Num())return;}
	
	for (AActor* Actor : Actors)
	{
		if (AWarp* MonSpawn = Cast<AWarp>(Actor))
		{
			if (MonSpawn->MonSpawnType ==  SpawnType)
			{
				TestPlayer->SetActorLocation(MonSpawn->GetActorLocation() + FVector(250,0,1000));
			}
		}
	}
}

void UUiPortal::WarpCotnroller(EMonSpawnType SpawnType)
{
	if (APlayerController* PcController = GetWorld()->GetFirstPlayerController())
	{
		if (APLAIPlayerController* pc = Cast<APLAIPlayerController>(PcController))
		{
			pc->Server_WarpPlayer(SpawnType);
		}
	}
}

void UUiPortal::OnButton_Village()
{
	WarpCotnroller(EMonSpawnType::Village);
	// WarpTestPlayer(EMonSpawnType::Village);
}

void UUiPortal::OnButton_Mountain()
{
	WarpCotnroller(EMonSpawnType::Mountain);
	// WarpTestPlayer(EMonSpawnType::Mountain);
}

void UUiPortal::OnButton_Dessert()
{
	WarpCotnroller(EMonSpawnType::Desert);
	// WarpTestPlayer(EMonSpawnType::Desert);
}

void UUiPortal::OnButton_Cave()
{
	WarpCotnroller(EMonSpawnType::Dungeon);
	// WarpTestPlayer(EMonSpawnType::Dungeon);
}

void UUiPortal::OnButton_OpenMap()
{
	if (bOpenMap == false)
	{
		WorldMap->SetVisibility(ESlateVisibility::Visible);
		bOpenMap = true;
	}
	else
	{
		WorldMap->SetVisibility(ESlateVisibility::Hidden);
		bOpenMap = false;
	}
}



// Warp->SetOwner(TestPlayer);
// Warp->Server_WarpPlayer(TestPlayer);
// UE_LOG(LogTemp,Warning,TEXT("UIPortal OnButton Village 실행 오너 누구? [%s] TestPlayer 이름은 [%s]"),*Warp->GetOwner()->GetName(),*TestPlayer->GetName());
// WarpTestPlayer(EMonSpawnType::Village);
// DrawDebugSphere(GetWorld(),Warp->GetOwner()->GetActorLocation(),50,12,FColor::Red,false,1.5);