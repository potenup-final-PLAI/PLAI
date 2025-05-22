// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcNet.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Components/TextBlock.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "PLAI/Item/Login/LoginComp.h"
#include "PLAI/Item/UI/Main/UiMain.h"


// Sets default values
ANpcNet::ANpcNet()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANpcNet::BeginPlay()
{
	Super::BeginPlay();
	
	NpcNameString = TEXT("Bass");
}

// Called every frame
void ANpcNet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ANpcNet::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ANpcNet::OpenQuest()
{
    UIPost = CreateWidget<UUiPost>(GetWorld(),UIPostFactory);
	UIPost->AddToViewport();
	UIPost->OnNetPost.BindUObject(this,&ANpcNet::NetPost);
	NpcUiMaster = Cast<UUserWidget>(UIPost);
	// if (!NpcUiMaster){
	// 	UE_LOG(LogTemp,Error,TEXT("NpcNet NpcUiMaster 생성 실패"));
	// }
	// else
	// {
	// 	UE_LOG(LogTemp,Error,TEXT("NpcNet NpcUiMaster 생성 성공"));
	// };
}

void ANpcNet::NetPost(FString String)
{
	FHttpRequestRef HttpRequest = FHttpModule::Get().CreateRequest();
	FNgrok Ngrok;
	HttpRequest->SetURL(Ngrok.Ngrok + TEXT("/npc/chat"));
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader("Content-Type", "application/json");

	// UE_LOG(LogTemp,Warning,TEXT("NpcNet String받은값 무엇%s"),*String)
	FString JsonString;
	FNpcStructPost NpcStruct;
	NpcStruct.question = String;
	NpcStruct.personality = personality;
	
	FJsonObjectConverter::UStructToJsonObjectString(NpcStruct,JsonString);
	HttpRequest->SetContentAsString(JsonString);
	HttpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
	{
		if (bSucceeded)
		{
			FNpcStructGet NpcStruct;
			FString JsonString = HttpResponse->GetContentAsString();
			FJsonObjectConverter::JsonObjectStringToUStruct(JsonString,&NpcStruct);
			// UE_LOG(LogTemp, Warning, TEXT("ANpcNet::NetPost 성공함 JsonString은? [%s] 서버코드 %d"),*JsonString,HttpResponse->GetResponseCode());
			UIPost->TextGet->SetText(FText::FromString(NpcStruct.response));
		}
	});
	HttpRequest->ProcessRequest();
}

