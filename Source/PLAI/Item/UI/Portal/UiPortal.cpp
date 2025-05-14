// Fill out your copyright notice in the Description page of Project Settings.

#include "UiPortal.h"

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
	UE_LOG(LogTemp,Warning,TEXT("UiPortal 어디소환중? [%s]"),*UEnum::GetValueAsString(SpawnType))
	
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWarp::StaticClass(), Actors);
	
	if (Actors.Num() == 0){UE_LOG(LogTemp,Warning,TEXT("UiPortal 없냐? %d"),Actors.Num())return;}
	
	for (AActor* Actor : Actors)
	{
		// UE_LOG(LogTemp,Display,TEXT("UiPortal Portal 이름은? [%s]"),*Actor->GetName());
		if (AWarp* MonSpawn = Cast<AWarp>(Actor))
		{
			if (MonSpawn->MonSpawnType ==  SpawnType)
			{
				TestPlayer->SetActorLocation(MonSpawn->GetActorLocation() + FVector(250,0,1000));
			}
		}
	}
}

void UUiPortal::OnButton_Village()
{
	WarpTestPlayer(EMonSpawnType::Village);
}

void UUiPortal::OnButton_Mountain()
{
	WarpTestPlayer(EMonSpawnType::Mountain);
}

void UUiPortal::OnButton_Dessert()
{
	WarpTestPlayer(EMonSpawnType::Desert);
}

void UUiPortal::OnButton_Cave()
{
	WarpTestPlayer(EMonSpawnType::Dungeon);
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
