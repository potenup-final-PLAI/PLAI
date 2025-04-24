// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcNet.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
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
}

void ANpcNet::NetPost(FString String)
{
	UE_LOG(LogTemp, Display, TEXT("ANpcNet::NetPost%s"), *String);
	FHttpRequestRef HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL("http://192.168.10.96:8054/npc/chat");
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader("Content-Type", "application/json");

	FNpcStructNetPost NpcStruct;
	NpcStruct.question = String;
	
	FString JsonString;
	
	FJsonObjectConverter::UStructToJsonObjectString(NpcStruct,JsonString);
	HttpRequest->SetContentAsString(JsonString);
	HttpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
	{
		if (bSucceeded)
		{
			FNpcStructNetGet NpcStruct;
			FString JsonString = HttpResponse->GetContentAsString();
			FJsonObjectConverter::UStructToJsonObjectString(NpcStruct,JsonString);
			UE_LOG(LogTemp, Display, TEXT("ANpcNet::NetPost 실패함"));
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("ANpcNet::NetPost 실패함"));
		}
	}
    );
	HttpRequest->ProcessRequest();
}

