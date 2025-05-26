// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGi.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Player/BattlePlayer.h"


void UWorldGi::Init()
{
	Super::Init();

	// 현재 사용하는 서브시스템을 가져오자
	IOnlineSubsystem* Subsys = IOnlineSubsystem::Get();
	if (Subsys)
	{
		SessionInterface = Subsys->GetSessionInterface();

		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this,&UWorldGi::OnCreateSessionComplete);
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this,&UWorldGi::OnFindSessionComplete);
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this,&UWorldGi::OnJoinSessionComplete);
	}
}

void UWorldGi::CreateSession(FString displayName, int32 playerCount)
{
	// 세션을 만들기 위한 옵션 설정
	FOnlineSessionSettings SessionSettings;
	
	FName SubSysName = IOnlineSubsystem::Get()->GetSubsystemName();

	// SessionSettings.bIsLANMatch = true;
	SessionSettings.bIsLANMatch = SubSysName.IsEqual(FName(TEXT("NULL")));
	
	UE_LOG(LogTemp, Error, TEXT("WorldGi = Creating Session... 서브시스템 이름 [%s]"),*SubSysName.ToString());

	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.NumPublicConnections = playerCount;
	
	// 커스텀 정보를 넣자
	SessionSettings.Set(FName(TEXT("DP_NAME")), displayName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	SessionInterface->CreateSession(0, FName(displayName), SessionSettings);
}
void UWorldGi::OnCreateSessionComplete(FName sessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("WorldGi = OnCreateSessionComplete 세션성공 [%s]"),*sessionName.ToString());
	
		GetWorld()->ServerTravel(TEXT("/Game/Mk_Item/Mk_WorldPartition?listen"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WorldGi = OnCreateSessionComplete 세션실패 [%s]"),*sessionName.ToString());
	}
}

void UWorldGi::FindOtherSession()
{
	UE_LOG(LogTemp,Warning,TEXT("WorldGI 세션 검색 시작"))

	// SessionSearch 만들자
	SessionSearch = MakeShared<FOnlineSessionSearch>();
	
	FName SubSysName = IOnlineSubsystem::Get()->GetSubsystemName();
	UE_LOG(LogTemp, Warning, TEXT("WorldGi = Finding Session... 서브시스템 이름 [%s]"),*SubSysName.ToString());
	SessionSearch->bIsLanQuery = SubSysName.IsEqual(FName(TEXT("NULL")));
	
	// 어떤 옵션을 기준으로 검색
	SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	SessionSearch->MaxSearchResults = 100;
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UWorldGi::OnFindSessionComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		// JS 추가
		// if (!SessionSearch.IsValid())
		// {
		// 	UE_LOG(LogTemp, Error, TEXT("WorldGI: SessionSearch가 유효하지 않음"));
		// 	return;
		// }
		
		int32 Rand = FMath::RandRange(10,100);
		UE_LOG(LogTemp,Warning,TEXT("WorldGI 세션 검색 성공 랜덤값[%d]"),Rand)

		auto Results = SessionSearch->SearchResults;
	
		UE_LOG(LogTemp, Warning, TEXT("WorldGI 검색된 세션 개수: %d"), SessionSearch->SearchResults.Num());
		
		for (int i = 0; i < Results.Num(); i++)
		{
			FString displayName;
			Results[i].Session.SessionSettings.Get(FName(TEXT("DP_NAME")),displayName);
			UE_LOG(LogTemp, Error, TEXT("WorldGi = 세션 번째 [%d] 세션 이름 [%s]"),i,*displayName);

			//UISteamLobby ScrollBox AddChild에 달아놓았음
			// OnFindSession.ExecuteIfBound();
			OnFindSession.Broadcast(displayName);
		}
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("WorldGI 세션 검색 실패"))
	}
}

void UWorldGi::JoinOtherSession()
{
	auto results = SessionSearch->SearchResults;
	if(results.Num() == 0) return;

	// 세션 이름 가져오자.
	FString displayName;
	// 5.5 이슈
	// SessionSettings 의 bUseLobbiesIfAvailable 와 bUsesPresence 의 값이 false 로 되어있다.
	results[0].Session.SessionSettings.bUseLobbiesIfAvailable = true;
	results[0].Session.SessionSettings.bUsesPresence = true;
	results[0].Session.SessionSettings.Get(FName(TEXT("DP_NAME")), displayName);

	// 세션 참여
	SessionInterface->JoinSession(0, FName(displayName), results[0]);
	
	// results[sessionIdx].Session.SessionSettings.Get(FName(TEXT("DP_NAME")), displayName);
	// // 세션 참여
	// sessionInterface->JoinSession(0, FName(displayName), results[sessionIdx]);
}

void UWorldGi::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	// 만약에 참여 성공했다면
	if (result == EOnJoinSessionCompleteResult::Success)
	{
		// 서버가 만들어 놓은 세션 URL 얻어 오자.
		FString url;
		SessionInterface->GetResolvedConnectString(sessionName, url);
		// 서버가 있는 맵으로 이동
		APlayerController* pc = GetWorld()->GetFirstPlayerController();
		pc->ClientTravel(url, TRAVEL_Absolute);
	}
}


void UWorldGi::EquipActor(AActor* MyActor)
{
	if (ABattlePlayer* Player = Cast<ABattlePlayer>(MyActor))
	{
		for (int32 i = 0; i < UserFullInfoGiStat.equipment_info.item_list.Num(); i++)
		{
			if (AItemMaster* Item = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory))
			{
				TArray<FName>Raws = ItemDataTable->GetRowNames();
				for (FName Raw : Raws)
				{
					if (Raw == UserFullInfoGiStat.equipment_info.item_list[i].item_id)
					{
						FRotator Rotator = MyActor->GetActorRotation();
						
						FItemStructTable* ItemStructTable = ItemDataTable->FindRow<FItemStructTable>(Raw,TEXT("WorldGi"));
						// 여기부터 장비 장착
						Item->StaticMesh->SetStaticMesh(ItemStructTable->StaticMesh);
						if (ItemStructTable->ItemIndex == 0)
						{
							Item->AttachToComponent(Player->meshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("hand_rSocket"));
							Item->AddActorWorldOffset(FVector(30, 5, -10));
							Item->AddActorLocalRotation(FRotator(-165, 10, 100));
						}
						else if (ItemStructTable->ItemIndex == 1)
						{
							Item->AttachToComponent(Player->meshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Chest"));
							Item->AddActorWorldOffset(FVector(-20, 0, -80));
							Item->SetActorRotation(Item->GetActorRotation() + FRotator(0, 90, 0));
							Item->SetActorScale3D(FVector(1.05, 1.05, 1.05));
						}
						else if (ItemStructTable->ItemIndex == 2)
						{
							Item->AttachToComponent(Player->meshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("headSocket"));

							Item->AddActorWorldOffset(FVector(-0, 0, 10));
							
							Item->AddActorLocalRotation(FRotator(0, -90, 90));
							
							Item->SetActorScale3D(FVector(0.8,0.8,0.8));
						} 
						else if (ItemStructTable->ItemIndex == 3)
						{
							Item->AttachToComponent(Player->meshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("FX_Ult_Passive"));

							Item->AddActorWorldOffset(FVector(-0, 0, -130));
							Item->AddActorLocalRotation(FRotator(0, 100, 0));
						}
						Item->StaticMesh->SetMaterial(0,ItemStructTable->Material);
					}
				}
			}
		}
	}
}

