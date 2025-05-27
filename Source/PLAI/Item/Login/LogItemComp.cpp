// Fill out your copyright notice in the Description page of Project Settings.


#include "LogItemComp.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "LoginComp.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Character/UIChaStat.h"
#include "PLAI/Item/UI/Inventory/EquipInven/EquipInven.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemInven.h"


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

	if (TestPlayer && TestPlayer->IsLocallyControlled() && PC && PC->WasInputKeyJustPressed(EKeys::L))//장비창 불러오기
	{
		UE_LOG(LogTemp, Warning, TEXT("LogItemComp::L키 장비창 불러오기"));
		GetEquipInfo();
		GetInvenInfo();
		GetUserLevel();
	}

	if (TestPlayer && TestPlayer->IsLocallyControlled() && PC && PC->WasInputKeyJustPressed(EKeys::K))//레벨업 셋팅
	{
		UE_LOG(LogTemp, Warning, TEXT("LogItemComp::K키 레벨 초기화 불러오기"));
		TestPlayer->LoginComp->UserFullInfo.character_info.level = 1;
		TestPlayer->LoginComp->UserFullInfo.character_info.current_exp = 0;
		TestPlayer->LoginComp->UserFullInfo.character_info.max_exp = 1000;
		TestPlayer->LoginComp->UserFullInfo.character_info.position.x = 0;
		TestPlayer->LoginComp->UserFullInfo.character_info.position.y = 0;
		TestPlayer->LoginComp->UserFullInfo.character_info.position.z = 0;
		TestPlayer->LoginComp->UserFullInfo.inventory_info.gold = 0;
		TestPlayer->InvenComp->MenuInven->Wbp_ItemGold->Gold->SetText(FText::AsNumber(TestPlayer->LoginComp->UserFullInfo.inventory_info.gold));
		TestPlayer->InvenComp->MenuInven->WBP_ItemInven->WbpItemGold->Gold->SetText(FText::AsNumber(TestPlayer->LoginComp->UserFullInfo.inventory_info.gold));
		TestPlayer->InvenComp->GetExp(500);

		for (UWidget* Widget : TestPlayer->InvenComp->MenuInven->WBP_ItemInven->WrapBox->GetAllChildren())
		{
			if (USlot* Slot = Cast<USlot>(Widget))
			{
				Slot->ItemStructTable = FItemStructTable();
				FSlateBrush Brush;
				Brush.SetResourceObject(nullptr);
				Brush.DrawAs = ESlateBrushDrawType::Type::NoDrawType;
				Slot->SlotImage->SetBrush(Brush);
			}
		}
	}
}

void ULogItemComp::GetEquipInfo()
{
	if (!TestPlayer->IsLocallyControlled()){UE_LOG(LogTemp,Warning,TEXT("LogItemComp GetInvenInfo localplayer 아님 리턴")) return;}

	// 장비 보낼떄 구조체
	FPostEquipId PostEquipId;
	Fequipment_info equipment_info;
	
	for (UWidget* widget : TestPlayer->InvenComp->MenuInven->WBP_EquipInven->LeftBox->GetAllChildren())
	{
		if (USlotEquip* SlotEquip = Cast<USlotEquip>(widget))
		{
			int32 index = TestPlayer->InvenComp->MenuInven->WBP_EquipInven->LeftBox->GetChildIndex(SlotEquip);
			int32 idx = 0;
			
			if (SlotEquip->ItemStructTable.ItemTop == -1)
			{
				UE_LOG(LogTemp,Warning,TEXT("LogItemComp GetEquipInfo 장비슬롯 있음 인덱스 무엇?%d"),
					TestPlayer->InvenComp->MenuInven->WBP_EquipInven->LeftBox->GetChildIndex(SlotEquip))
				continue;
			}
			// 장비 보내기 준비
			equipment_info.options.hp = SlotEquip->ItemStructTable.ItemStructStat.item_SHI;
			equipment_info.options.attack = SlotEquip->ItemStructTable.ItemStructStat.item_ATK;
			equipment_info.options.defense = SlotEquip->ItemStructTable.ItemStructStat.item_DEF;
			equipment_info.options.resistance = SlotEquip->ItemStructTable.ItemStructStat.item_RES;
			equipment_info.options.critical_rate = SlotEquip->ItemStructTable.ItemStructStat.Item_CRIT;
			equipment_info.options.critical_damage = SlotEquip->ItemStructTable.ItemStructStat.item_CRITDMG;
			equipment_info.item_id = SlotEquip->ItemStructTable.Item_Id;
			
			PostEquipId.equipment_info.Add(equipment_info);
			
			FitemInfo iteminfo;
			iteminfo.item_id = SlotEquip->ItemStructTable.Item_Id;
			TestPlayer->LoginComp->UserFullInfo.equipment_info.item_list.Add(iteminfo); 
			// TestPlayer->LoginComp->UserFullInfo.equipment_info.item_list[index].item_id = SlotEquip->ItemStructTable.Item_Id;
		}
	}
	PostEquipId.character_id = TestPlayer->LoginComp->UserFullInfo.character_info.character_id;
	
	UE_LOG(LogTemp, Display, TEXT("LogItemComp GetEuqipInfo Test->Character Id = PostEquipId  [%s]"),*TestPlayer->LoginComp->UserFullInfo.character_info.character_id);
	UE_LOG(LogTemp, Display, TEXT("LogItemComp GetEuqipInfo 캐릭터 아이디 구조체 LoginComp %s"),*TestPlayer->LoginComp->UserFullInfo.user_id);
	
	FString jsonString;
	FJsonObjectConverter::UStructToJsonObjectString(PostEquipId,jsonString);
	UE_LOG(LogTemp, Display, TEXT("LogItemComp GetEuqipInfo PostEquipId: %s"), *jsonString);
	
	HttpEquipPost(jsonString);
}

void ULogItemComp::HttpEquipPost(FString JsonString)
{
	FHttpRequestRef httpRequest = FHttpModule::Get().CreateRequest();
	FNgrok Ngrok;
	httpRequest->SetURL(Ngrok.Ngrok + TEXT("/items/upsert/equipment"));
	httpRequest->SetVerb("POST");
	httpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	httpRequest->SetContentAsString(JsonString);
	httpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bProcessedSuccessfully)
	{
		if (bProcessedSuccessfully)
		{
			FString JsonString = HttpResponse->GetContentAsString();
			UE_LOG(LogTemp,Warning,TEXT("LogItemComp 장비정보 집어넣기 성공 %s"),*JsonString);
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("LogItemComp 장비정보 집어넣기 실패"));
		}
	});
	httpRequest->ProcessRequest();
}


void ULogItemComp::GetInvenInfo()
{
	if (!TestPlayer->IsLocallyControlled()){UE_LOG(LogTemp,Warning,TEXT("LogItemComp GetInvenInfo localplayer 아님 리턴")) return;}
	
	FPostInvenId PostInvenId;
	FInvenTory_Info InvenToryInfo;

	for (UWidget* Widget : TestPlayer->InvenComp->MenuInven->WBP_ItemInven->WrapBox->GetAllChildren())
	{
		if (USlot* Slot = Cast<USlot>(Widget))
		{
			int index = TestPlayer->InvenComp->MenuInven->WBP_ItemInven->WrapBox->GetChildIndex(Slot);
			if (Slot->ItemStructTable.ItemTop == -1 || Slot->ItemStructTable.ItemTop == 3)
			{
				// UE_LOG(LogTemp,Warning,TEXT("LogItemComp, 여기 인벤슬롯 비었으니 넘어감 / %d"),index);
				continue;
			}
			// 아직 서버 아이템 테이블에 안들어가있음
			if (Slot->ItemStructTable.ItemTop == 0 && Slot->ItemStructTable.ItemIndex == 0 &&
				Slot->ItemStructTable.ItemIndexType == 1)
			{
				continue;
			}
			
			InvenToryInfo.item_id = Slot->ItemStructTable.Item_Id;
			InvenToryInfo.Counts = Slot->ItemStructTable.ItemNum;

			InvenToryInfo.options.attack = Slot->ItemStructTable.ItemStructStat.item_ATK;
			InvenToryInfo.options.defense = Slot->ItemStructTable.ItemStructStat.item_DEF;
			InvenToryInfo.options.resistance = Slot->ItemStructTable.ItemStructStat.item_RES;
			InvenToryInfo.options.critical_damage = Slot->ItemStructTable.ItemStructStat.item_CRITDMG;
			InvenToryInfo.options.critical_rate = Slot->ItemStructTable.ItemStructStat.Item_CRIT;
			InvenToryInfo.options.hp = Slot->ItemStructTable.ItemStructStat.item_SHI;

			PostInvenId.Inventory_info.Add(InvenToryInfo);
		}
	}
	PostInvenId.character_id = TestPlayer->LoginComp->UserFullInfo.character_info.character_id;
	PostInvenId.Gold = TestPlayer->LoginComp->UserFullInfo.inventory_info.gold;

	FString jsonString;
	FJsonObjectConverter::UStructToJsonObjectString(PostInvenId,jsonString);
	UE_LOG(LogTemp,Warning,TEXT("LogItemComp 인벤토리 정보 보내기 JsonString / %s"),*jsonString);
	HttpInvenPost(jsonString);
}

void ULogItemComp::HttpInvenPost(FString JsonString)
{
	UE_LOG(LogTemp,Warning,TEXT("LogItemComp, 아이템 정보 넘기기 "))
    FNgrok Ngrok;
	FHttpRequestRef httpRequest = FHttpModule::Get().CreateRequest();
	httpRequest->SetURL(Ngrok.Ngrok + TEXT("/items/upsert/inventory"));
	httpRequest->SetVerb("POST");
	httpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	httpRequest->SetContentAsString(JsonString);
	httpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bProcessedSuccessfully)
	{
		if (bProcessedSuccessfully)
		{
			FString JsonString = HttpResponse->GetContentAsString();
			UE_LOG(LogTemp,Warning,TEXT("LogItemComp Response->GetAsString %s"),*JsonString)
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("LogItemComp 인벤토리 정보 넘기기 실패 "))
		}
	});
	httpRequest->ProcessRequest();
}

void ULogItemComp::GetUserLevel()
{
	if (!TestPlayer->IsLocallyControlled()){UE_LOG(LogTemp,Warning,TEXT("LogItemComp GetInvenInfo localplayer 아님 리턴")) return;}
	
	FPutUserLevel PutUserLevel;
	PutUserLevel.character_id = TestPlayer->LoginComp->UserFullInfo.character_info.character_id;
	PutUserLevel.current_exp = TestPlayer->LoginComp->UserFullInfo.character_info.current_exp;
	PutUserLevel.level = TestPlayer->LoginComp->UserFullInfo.character_info.level;
	PutUserLevel.max_exp = TestPlayer->LoginComp->UserFullInfo.character_info.max_exp;
	PutUserLevel.position = TestPlayer->LoginComp->UserFullInfo.character_info.position;
	PutUserLevel.traits = TestPlayer->LoginComp->UserFullInfo.character_info.traits;
	
	PutUserLevel.position.x = TestPlayer->GetActorLocation().X;
	PutUserLevel.position.y = TestPlayer->GetActorLocation().Y;
	PutUserLevel.position.z = TestPlayer->GetActorLocation().Z;
	
	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(PutUserLevel,JsonString);
	HttpUserLevelPut(JsonString);
}

void ULogItemComp::HttpUserLevelPut(FString JsonString)
{
	FNgrok Ngrok;
	FHttpRequestRef httpRequest = FHttpModule::Get().CreateRequest();
	// httpRequest->SetURL(Ngrok.Ngrok + TEXT("/users/login"));
	// httpRequest->SetURL("https://919e-221-148-189-129.ngrok-free.app/service1/characters/update");
	httpRequest->SetURL(Ngrok.Ngrok + TEXT("/characters/update"));
	httpRequest->SetVerb("PUT");
	httpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	httpRequest->SetContentAsString(JsonString);

	httpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bProcessedSuccessfully)
	{
		if (bProcessedSuccessfully)
		{
			FString JsonString = HttpResponse->GetContentAsString();
			UE_LOG(LogTemp,Warning,TEXT("LogItemComp 머라옴 답변 [%s]"),*JsonString);
		}
	});
	
	httpRequest->ProcessRequest();
}



