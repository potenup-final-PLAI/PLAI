// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/Http/BattleHttpActor.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Battle/TurnSystem/PhaseManager.h"
#include "Battle/Util/DebugHeader.h"
#include "Enemy/BaseEnemy.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"


// Sets default values
ABattleHttpActor::ABattleHttpActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ABattleHttpActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABattleHttpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(
	// 	EKeys::One))
	// {
	// 	HttpPost();
	// }
}

void ABattleHttpActor::HttpPost(FEnvironmentState environmentState,FBattleTurnState battleTurnState,ABaseBattlePawn* unit)
{
	if (IsEmptyEnvironmentState(environmentState) && IsEmptyBattleTurnState(
		battleTurnState))
	{
		UE_LOG(LogTemp, Warning, TEXT("Both states are empty."));
		return;
	}

	bool bHasEnv = !IsEmptyEnvironmentState(environmentState);
	bool bHasTurn = !IsEmptyBattleTurnState(battleTurnState);


	// 서버에게 요청하는 객체 만들자.
	FHttpRequestRef httpRequest = FHttpModule::Get().CreateRequest();
	// Start API 일때
	if (bHasEnv)
	{
		// 요청 URL - 서버가 알려줌
		httpRequest->SetURL(TEXT("https://0b55-221-148-189-129.ngrok-free.app/service1/battle/start"));
	}
	// Action API 일때
	else if (bHasTurn)
	{
		// 요청 URL - 서버가 알려줌
		httpRequest->SetURL(TEXT("https://0b55-221-148-189-129.ngrok-free.app/service1/battle/action"));
	}
	// 요청 방식
	httpRequest->SetVerb(TEXT("POST"));
	// 헤더를 설정
	httpRequest->SetHeader(TEXT("Content-type"), TEXT("application/json"));

	FString jsonString;

	// 두 값중 True인 값에 JsonString 채워짐
	if (bHasEnv)
	{
		FJsonObjectConverter::UStructToJsonObjectString(environmentState, jsonString);
	}
	if (bHasTurn)
	{
		FJsonObjectConverter::UStructToJsonObjectString(battleTurnState, jsonString);
	}

	if (auto* phaseManager = Cast<AUPhaseManager>(GetWorld()->GetGameState()))
	{
		// 현재 페이즈가 None이라면
		if (phaseManager->currentPhase == EBattlePhase::None)
		{
			phaseManager->SetPhase(EBattlePhase::BattleStart);
		}
	}
	httpRequest->SetContentAsString(jsonString);

	UE_LOG(LogTemp, Warning, TEXT("%s"), *jsonString);
	// 서버에게 요청을 한 후 응답이오면 호출되는 함수 등록
	httpRequest->OnProcessRequestComplete().BindLambda([=, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bProcessedSuccessfully)
	{
		NET_PRINTLOG(TEXT("HttpPost 응답, %s"), (unit ? *unit->MyName : TEXT("unit is nullptr")));
		//GetResponseCode : 200 - 성공, 400번대, 500번대 - 오류
		// 응답이 오면 실행
		// 성공
		if (bProcessedSuccessfully && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
		{
			// 최초 데이터를 보내면 Success 값이 잘 들어오는지만 판단
			FString jsonData = Response->GetContentAsString();
			UE_LOG(LogTemp, Warning, TEXT("%s"), *jsonData);
			
			if (bHasEnv)
			{
				if (auto* phaseManager = Cast<AUPhaseManager>(GetWorld()->GetGameState()))
				{
					// 현재 페이즈가 None이라면
					if (phaseManager->currentPhase == EBattlePhase::None)
					{
						phaseManager->SetPhase(EBattlePhase::BattleStart);
					}
				}
			}
			if (bHasTurn)
			{
				FActionRequest ParsedRequest;
				if (FJsonObjectConverter::JsonObjectStringToUStruct(jsonData, &ParsedRequest, 0, 0))
				{
					if (auto* phaseManager = Cast<AUPhaseManager>(GetWorld()->GetGameState()))
					{
						// Turn 진행 중이라면
						if (phaseManager->currentPhase == EBattlePhase::TurnProcessing)
						{
							// id 값이 현재 유닛과 같다면
							if (unit && unit->GetName() == ParsedRequest.current_character_id)
							{
								if (ABaseEnemy* enemy = Cast<ABaseEnemy>(unit))
								{
									// enemy 행동 시작
									enemy->ProcessAction(ParsedRequest);
								}
							}
						}
					}
				}
				else
				{
					UE_LOG(LogTemp, Error,TEXT("Failed to parse ActionRequest JSON"));
				}
			}
		}
		// 실패
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("통신 실패 : %d"), Response->GetResponseCode());
		}
	});
	
	// 요청을 보내자.
	NET_PRINTLOG(TEXT("HttpPost 요청, %s"), (unit ? *unit->MyName : TEXT("unit is nullptr")));
	httpRequest->ProcessRequest();
}

bool ABattleHttpActor::IsEmptyEnvironmentState(const FEnvironmentState& state)
{
	return state.characters.IsEmpty() && state.terrain.IsEmpty() && state.
		weather.IsEmpty();
}

bool ABattleHttpActor::IsEmptyBattleTurnState(const FBattleTurnState& state)
{
	return state.characters.IsEmpty();
}
