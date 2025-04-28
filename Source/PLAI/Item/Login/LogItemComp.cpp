// Fill out your copyright notice in the Description page of Project Settings.


#include "LogItemComp.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "LoginComp.h"
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
	PC = Cast<APlayerController>(TestPlayer->GetController());
	
}


// Called every frame
void ULogItemComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TestPlayer->IsLocallyControlled() && PC->WasInputKeyJustPressed(EKeys::L))//장비창 불러오기
	{
		UE_LOG(LogTemp, Warning, TEXT("LogItemComp::L키 장비창 불러오기"));
		GetEquipInfo();
	}

	// ...
}

void ULogItemComp::HttpEquipPost(FPostEquipId PostEquipId)
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
	FPostEquipId PostEquipId;
	Fequipment_info equipment_info;
	
	for (UWidget* widget : TestPlayer->InvenComp->MenuInven->WBP_EquipInven->LeftBox->GetAllChildren())
	{
		if (USlotEquip* SlotEquip = Cast<USlotEquip>(widget))
		{
			equipment_info.options.hp = SlotEquip->ItemStructTable.ItemStructStat.item_SHI;
			equipment_info.options.attack = SlotEquip->ItemStructTable.ItemStructStat.item_ATK;
			equipment_info.options.defense = SlotEquip->ItemStructTable.ItemStructStat.item_DEF;
			equipment_info.options.resistance = SlotEquip->ItemStructTable.ItemStructStat.item_RES;
			equipment_info.options.critical_rate = SlotEquip->ItemStructTable.ItemStructStat.Item_CRIT;
			equipment_info.options.critical_damage = SlotEquip->ItemStructTable.ItemStructStat.item_CRITDMG;
			equipment_info.item_id = SlotEquip->ItemStructTable.Item_Id;
			
			PostEquipId.equipment_info.Add(equipment_info);
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("LogItemComp GetEuqipInfo 실패함"));
		}
	}
	PostEquipId.character_id = TestPlayer->LoginComp->character_id;
	FString jsonString;
	FJsonObjectConverter::UStructToJsonObjectString(PostEquipId,jsonString);
	UE_LOG(LogTemp, Display, TEXT("LogItemComp GetEuqipInfo PostEquipId: %s"), *jsonString);
}

