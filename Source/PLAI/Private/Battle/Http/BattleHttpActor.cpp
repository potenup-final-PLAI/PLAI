// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/Http/BattleHttpActor.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"


// Sets default values
ABattleHttpActor::ABattleHttpActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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

	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(
		EKeys::One))
	{
		HttpPost();
	}
}

void ABattleHttpActor::HttpPost()
{
	// 서버에게 요청하는 객체 만들자.
	FHttpRequestRef httpRequest = FHttpModule::Get().CreateRequest();
	// 요청 URL - 서버가 알려줌
	httpRequest->SetURL(TEXT("http://192.168.10.96:8054/battle/action"));
	// 요청 방식
	httpRequest->SetVerb(TEXT("POST"));
	// 헤더를 설정
	httpRequest->SetHeader(TEXT("Content-type"), TEXT("application/json"));

	// 서버에게 보내고 싶은 데이터 값 (Json) 
	// battle start 구조체 부분 더미데이터 부분
	FCharacterDetail startInfo;
	startInfo.id = TEXT("monster1");
	startInfo.name = TEXT("아이스 골램");
	startInfo.type = TEXT("monster");
	startInfo.personality = TEXT("충동적");
	startInfo.skills.Add(TEXT("얼음 창"));
	startInfo.skills.Add(TEXT("수비 자세"));

	FEnvironmentState environmentInfo;
	environmentInfo.characters.Add(startInfo);
	environmentInfo.terrain = TEXT("설산");
	environmentInfo.weather = TEXT("눈");

	// action 더미데이터 구조체 부분
	FCharacterInfo info;
	info.id = TEXT("monster1");
	info.position.Add(1);
	info.position.Add(10);
	info.hp = 100;
	info.ap = 0;

	battleState.cycle = 2;
	battleState.turn = 5;
	battleState.target_monster_id = TEXT("monster1");
	battleState.characters.Add(info);


	FString jsonString;
	FJsonObjectConverter::UStructToJsonObjectString(battleState, jsonString);

	UE_LOG(LogTemp, Warning, TEXT("%s"), *jsonString);

	httpRequest->SetContentAsString(jsonString);

	// 서버에게 요청을 한 후 응답이오면 호출되는 함수 등록
	httpRequest->OnProcessRequestComplete().BindLambda(
		[this](FHttpRequestPtr Request, FHttpResponsePtr Response,
		       bool bProcessedSuccessfully)
		{
			//GetResponseCode : 200 - 성공, 400번대, 500번대 - 오류

			// 응답이 오면 실행
			// 성공
			if (bProcessedSuccessfully && EHttpResponseCodes::IsOk(
				Response->GetResponseCode()))
			{
				FString jsonString = Response->GetContentAsString();
				UE_LOG(LogTemp, Warning, TEXT("%s"), *jsonString);
			}
			// 실패
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("통신 실패 : %d"),
				       Response->GetResponseCode());
			}
		});

	// 요청을 보내자.
	httpRequest->ProcessRequest();
}
