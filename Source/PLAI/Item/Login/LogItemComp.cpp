// Fill out your copyright notice in the Description page of Project Settings.


#include "LogItemComp.h"
#include "HttpModule.h"
#include "Components/VerticalBox.h"
#include "Interfaces/IHttpRequest.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/UI/Inventory/EquipInven/EquipInven.h"


// Sets default values for this component's properties
ULogItemComp::ULogItemComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULogItemComp::BeginPlay()
{
	Super::BeginPlay();
	TestPlayer = Cast<ATestPlayer>(GetOwner());
	// ...
	
}


// Called every frame
void ULogItemComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void ULogItemComp::HttpEquipPost()
{
	FHttpRequestRef httpRequest = FHttpModule::Get().CreateRequest();
	
	httpRequest->SetURL(TEXT("http://192.168.10.96:8054/me/"));
	httpRequest->SetVerb("POST");
	httpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	FString JsonString;
	// FMeStruct MeStruct;
	
	// MeStruct.user_id = User_id;
	
	// FJsonObjectConverter::UStructToJsonObjectString(MeStruct,JsonString);
	httpRequest->SetContentAsString(JsonString);
	httpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bProcessedSuccessfully)
	{
		if (bProcessedSuccessfully)
		{
			// FString JsonString = HttpResponse->GetContentAsString();
			// UE_LOG(LogTemp,Warning,TEXT("로그인컴프 나의정보 조회 성공 %s"),*JsonString);
		}
		else
		{
			// UE_LOG(LogTemp,Warning,TEXT("로그인컴프 나의정보 실패 %d"),HttpResponse->GetResponseCode());
		}
	});
	httpRequest->ProcessRequest();
}

void ULogItemComp::GetEquipInfo()
{
	for (UWidget* widget : TestPlayer->InvenComp->MenuInven->WBP_EquipInven->RightBox->GetAllChildren())
	{
		if (USlotEquip* SlotEquip = Cast<USlotEquip>(widget))
		{
			Foptions options;
			options.hp = SlotEquip->ItemStructTable.ItemStructStat.item_SHI;
			options.attack = SlotEquip->ItemStructTable.ItemStructStat.item_SHI;
			options.defense = SlotEquip->ItemStructTable.ItemStructStat.item_SHI;
			options.resistance = SlotEquip->ItemStructTable.ItemStructStat.item_SHI;
			options.critical_rate = SlotEquip->ItemStructTable.ItemStructStat.item_SHI;
		}
	}
}

