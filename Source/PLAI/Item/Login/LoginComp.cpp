// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginComp.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "UserStruct.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/Npc/NpcNet.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Inventory/EquipInven/EquipInven.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemInven.h"
#include "PLAI/Item/UI/Main/UiSign.h"

// Sets default values for this component's properties
ULoginComp::ULoginComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULoginComp::BeginPlay()
{
	Super::BeginPlay();
	
    TestPlayer = Cast<ATestPlayer>(GetOwner());
	
	if (TestPlayer->IsLocallyControlled())
	{
		UiMain = CreateWidget<UUiMain>(GetWorld(),UiMainFactory);
		UiMain->AddToViewport();
		UiMain->LoginComp = this;
		UiMain->WbpUiSign->LoginComp = this;
	}
}


// Called every frame
void ULoginComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DrawDebugString(GetWorld(),TestPlayer->GetActorLocation() + FVector(0, 0, 100),
	FString::Printf(TEXT("LoginComp 나의 UserId [%s] \n "
					  "나의 CharacterId [%s]"),*User_id, *UserFullInfo.character_info.character_id),nullptr,FColor::Red,0.f,false);

	if (APlayerController* PC = Cast<APlayerController>(TestPlayer->GetController()))
	{ if (PC->WasInputKeyJustPressed(EKeys::One)) // 1 캐릭터 생성 요청
	{   UE_LOG(LogTemp,Display,TEXT("로그인 컴프 1키 User 구조체 UserId 정보조회 %s"),*UserFullInfo.user_id);
		UE_LOG(LogTemp,Display,TEXT("로그인 컴프 1키 User Character Id 정보조회 %s"),*character_id);
	}
	}
	
	if (APlayerController* PC = Cast<APlayerController>(TestPlayer->GetController()))
	{ if (PC->WasInputKeyJustPressed(EKeys::C)) // C 캐릭터 생성 요청
		{   UE_LOG(LogTemp,Display,TEXT("Input C 캐릭터 생성 요청 Key JustPressed"));
		    HttpCreatePost();
		}
	}

	if (APlayerController* PC = Cast<APlayerController>(TestPlayer->GetController()))
	{ if (PC->WasInputKeyJustPressed(EKeys::M)) // M 캐릭터 생성 내 정보 요청
	{   UE_LOG(LogTemp,Display,TEXT("Input M 내 정보 주셈 Key JustPressed"));
		HttpMePost();
	}
	}

	if (APlayerController* PC = Cast<APlayerController>(TestPlayer->GetController()))
	{ if (PC->WasInputKeyJustPressed(EKeys::N)) // N 내아이템 주셈
	{   UE_LOG(LogTemp,Display,TEXT("Input N 내 아이템 주셈 Key JustPressed"));
		LoadEquipItem();
	}
	}
	

	if (APlayerController* PC = Cast<APlayerController>(TestPlayer->GetController()))
	{ if (PC->WasInputKeyJustPressed(EKeys::LeftMouseButton))
		{
		    FHitResult Hit;
		    PC->GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		    if (Hit.bBlockingHit)
		    {
			    if (ANpcNet * NpcNet = Cast<ANpcNet>(Hit.GetActor()))
			    {
			    	if (bQuest == true)
			    	{
			    		NpcNet->OpenQuest();
			    		bQuest = false;
			    	}
			    	else
			    	{
			    		NpcNet->UIPost->RemoveFromParent();
			    		bQuest = true;
			    	}
			    }
		    	else
		    	{
		    		UE_LOG(LogTemp, Error, TEXT("LoginComp NpcNet 없네"));
		    	}
		    }
		};
	}

	if (APlayerController* PC = Cast<APlayerController>(TestPlayer->GetController()))
	{
		if (PC->WasInputKeyJustPressed(EKeys::P))
		{
			float CurrentDistance = TestPlayer->CameraBoom->TargetArmLength;
			TestPlayer->CameraBoom->TargetArmLength = CurrentDistance + 300;
		}
	}
	if (APlayerController* PC = Cast<APlayerController>(TestPlayer->GetController()))
	{
		if (PC->WasInputKeyJustPressed(EKeys::O))
		{
			float CurrentDistance = TestPlayer->CameraBoom->TargetArmLength;
			TestPlayer->CameraBoom->TargetArmLength = CurrentDistance - 300;
		}
	}
}

void ULoginComp::HttpLoginPost()
{
	FHttpRequestRef httpRequest = FHttpModule::Get().CreateRequest();
	httpRequest->SetURL(TEXT("http://192.168.10.96:8054/users/login"));
	httpRequest->SetVerb("POST");
	httpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	FLoginStruct LoginStruct;
	
	LoginStruct.email = UiMain->LoginId->GetText().ToString();
	LoginStruct.password = UiMain->LoginPw->GetText().ToString();

	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(LoginStruct, JsonString);
	httpRequest->SetContentAsString(JsonString);
	httpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bProcessedSuccessfully)
	{
		FLoginStructGet LoginStructGet;
		if (bProcessedSuccessfully)
		{ FString JsonString = HttpResponse->GetContentAsString();
			UE_LOG(LogTemp, Warning, TEXT("로그인컴프 통신성공 로그인%s"), *JsonString);

			FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &LoginStructGet);
			if (LoginStructGet.user_id != TEXT("string"))
			{ OnLogin.ExecuteIfBound(true);
				UE_LOG(LogTemp, Warning, TEXT("로그인컴프 통신성공 로그인%s"),*LoginStructGet.user_id);
				User_id = LoginStructGet.user_id;
			}
			else
			{ OnLogin.ExecuteIfBound(false);
				UE_LOG(LogTemp, Warning, TEXT("로그인컴프 통신성공 로그인 실패")); }
		}
	});
	httpRequest->ProcessRequest();
}


void ULoginComp::HttpSignPost()
{
	FHttpRequestRef httpRequest = FHttpModule::Get().CreateRequest();
	
	httpRequest->SetURL(TEXT("http://192.168.10.96:8054/users/register"));
	httpRequest->SetVerb("POST");
	httpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	FLoginStruct LoginStruct;
	LoginStruct.email = UiMain->WbpUiSign->SignId->GetText().ToString();
	LoginStruct.password = UiMain->WbpUiSign->SignPw->GetText().ToString();

	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(LoginStruct, JsonString);
	httpRequest->SetContentAsString(JsonString);
	
	httpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bProcessedSuccessfully)
	{
		if (bProcessedSuccessfully)
		{
			FString JsonString = HttpResponse->GetContentAsString();
			UE_LOG(LogTemp,Warning,TEXT("로그인컴프 가입요청 성공 %s"),*JsonString);
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("로그인컴프 가입요청 실패 %d"),HttpResponse->GetResponseCode());
		}
	});
	httpRequest->ProcessRequest();
}


void ULoginComp::HttpMePost()
{
	FHttpRequestRef httpRequest = FHttpModule::Get().CreateRequest();
	
	httpRequest->SetURL(TEXT("http://192.168.10.96:8054/me/"));
	httpRequest->SetVerb("POST");
	httpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	FString JsonString;
	FMeStruct MeStruct;
	
	MeStruct.user_id = User_id;
	
	FJsonObjectConverter::UStructToJsonObjectString(MeStruct,JsonString);
	httpRequest->SetContentAsString(JsonString);
	httpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bProcessedSuccessfully)
	{
		if (bProcessedSuccessfully)
		{
			FString JsonString = HttpResponse->GetContentAsString();
			UE_LOG(LogTemp,Warning,TEXT("로그인컴프 나의정보 조회 성공 %s"),*JsonString);
			FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &UserFullInfo);
			FString GetJson;
			
			FJsonObjectConverter::UStructToJsonObjectString(UserFullInfo,GetJson);
			UE_LOG(LogTemp,Warning,TEXT("로그인컴프 나의정보 조회 Json변환 %s"),*GetJson);
			
			character_id = UserFullInfo.character_info.character_id;
			
			UE_LOG(LogTemp,Warning,TEXT("로그인컴프 Logitem Comp CharId%s"),*TestPlayer->LogItemComp->Char_id)

			// for (int32 i = 0; i < UserFullInfo.equipment_info.item_list.Num(); i++)
			// {
			// 	UE_LOG(LogTemp,Warning,TEXT("로그인컴프 장비이름%s"),*UserFullInfo.equipment_info.item_list[i].item_id)
			// }
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("로그인컴프 나의정보 실패 %d"),HttpResponse->GetResponseCode());
		}
	});
	httpRequest->ProcessRequest();
}

void ULoginComp::LoadEquipItem()
{
	for (int32 i = 0; i < UserFullInfo.equipment_info.item_list.Num() ; i++)
	{
		for (UWidget* Widget : TestPlayer->InvenComp->MenuInven->WBP_EquipInven->LeftBox->GetAllChildren())
		{
			if (USlotEquip* SlotEquip = Cast<USlotEquip>(Widget)) // 1번부터 5번까지 슬롯 돈다
			{
				TArray<FName>RawNames = SlotEquip->ItemTable->GetRowNames();
			    for (FName RawName : RawNames)
			    {
			    	FItemStructTable* ItemStructTable = SlotEquip->ItemTable->FindRow<FItemStructTable>(RawName,"로그인컴프");
			    	if (UserFullInfo.equipment_info.item_list[i].item_id == RawName &&
			    		static_cast<int32>(SlotEquip->SlotType) == ItemStructTable->ItemIndex)
			    	{
			    		TestPlayer->InvenComp->EquipItem(*ItemStructTable,SlotEquip->SlotType);
			    		SlotEquip->ItemStructTable = *ItemStructTable;
			    		SlotEquip->SlotImageUpdate();
			    		break;
			    	}
			    }
			}
		}
	}
	
		
	for (UWidget* Widget : TestPlayer->InvenComp->MenuInven->WBP_EquipInven->LeftBox->GetAllChildren())
	{
		if (USlotEquip* SlotEquip = Cast<USlotEquip>(Widget)) // 1번부터 5번까지 슬롯 돈다
		{
			
		}
	}



	
	// for (UWidget* Widget : TestPlayer->InvenComp->MenuInven->WBP_EquipInven->LeftBox->GetAllChildren())
	// {
	// 	if (USlotEquip* SlotEquip = Cast<USlotEquip>(Widget)) // 1번부터 5번까지 슬롯 돈다
	// 	{
 //            for (int32 i = 0; i < UserFullInfo.equipment_info.item_list.Num(); i++)
 //            {
 //            	TArray<FName>RawNames = SlotEquip->ItemTable->GetRowNames();
 //            	for (FName Raw : RawNames)
 //            	{
 //            		FItemStructTable* ItemStructTable = SlotEquip->ItemTable->FindRow<FItemStructTable>(Raw,("LoginComp 240"));
 //                    if (ItemStructTable->Item_Id == UserFullInfo.equipment_info.item_list[i].item_id)
 //                    {
 //                    	UE_LOG(LogTemp,Warning,TEXT("로그인컴프 ItemId는 %s"),*UserFullInfo.equipment_info.item_list[i].item_id)
 //                    	TestPlayer->InvenComp->EquipItem(*ItemStructTable,SlotEquip->SlotType);
 //                    	break;
 //                    }
 //            	}  
 //            }
	// 	}
	// }
}


//
//
// void ULoginComp::TransDataTable()
// {
// 	UE_LOG(LogTemp, Warning, TEXT("구조체 변경 시작"));
//
// 	UDataTable* OldDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Mk_Item/Dt_ItemStruct.Dt_ItemStruct"));
// 	OldDataTable->RowStruct = FItemStruct::StaticStruct();
// 	UDataTable* NewDataTable = NewObject<UDataTable>();
// 	NewDataTable->RowStruct = FItemStructTable::StaticStruct();
//
// 	const TArray<FName> RowNames = OldDataTable->GetRowNames();
// 	for (const FName& RowName : RowNames)
// 	{
// 		if (FItemStruct* SrcRow = OldDataTable->FindRow<FItemStruct>(RowName, TEXT("")))
// 		{
// 			FItemStructTable NewRow;
// 			NewRow.ItemTop = SrcRow->ItemTop;
// 			NewRow.ItemIndex = SrcRow->ItemIndex;
// 			NewRow.ItemIndexType = SrcRow->ItemIndexType;
// 			NewRow.ItemIndexDetail = SrcRow->ItemIndexDetail;
// 			NewRow.Name = SrcRow->Name;
// 			NewRow.NameType = SrcRow->NameType;
// 			NewRow.NameDetail = SrcRow->NameDetail;
// 			NewRow.ItemNum = SrcRow->ItemNum;
// 			NewRow.ItemStructStat = SrcRow->ItemStructStat;
// 			NewRow.ItemStructStatName = SrcRow->ItemStructStatName;
//
// 			NewDataTable->AddRow(RowName, NewRow);
//
// 			UE_LOG(LogTemp, Warning, TEXT("데이터테이블 변환중: %s"), *NewRow.ItemStructStatName.item_SHI);
// 		}
// 	}
// 	UE_LOG(LogTemp, Warning, TEXT("Row Count: %d"), NewDataTable->GetRowNames().Num());
// 	
// 		FString TableCSV = NewDataTable->GetTableAsCSV(EDataTableExportFlags::None);
// 		FFileHelper::SaveStringToFile(TableCSV, *(FPaths::ProjectDir() + "NewDataTable.csv"));
// 		
// 		UE_LOG(LogTemp, Warning, TEXT("CSV 저장 완료"));
// }

void ULoginComp::HttpCreatePost()
{
	FHttpRequestRef httpRequest = FHttpModule::Get().CreateRequest();
	
	httpRequest->SetURL(TEXT("http://192.168.10.96:8054/characters/create"));
	httpRequest->SetVerb("POST");
	httpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	FCreateStruct CreateStruct;
	CreateStruct.user_id = User_id;

	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(CreateStruct, JsonString);
	httpRequest->SetContentAsString(JsonString);
	
	httpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bProcessedSuccessfully)
	{
		if (bProcessedSuccessfully)
		{
			FCreateStructGet CreateStructGet;
			FString JsonString = HttpResponse->GetContentAsString();
			UE_LOG(LogTemp,Warning,TEXT("로그인컴프 캐릭터 생성 성공 %s"),*JsonString);
			FJsonObjectConverter::UStructToJsonObjectString(CreateStructGet,JsonString);
			character_id = CreateStructGet.character_id;
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("로그인컴프 캐릭터 생성 실패 %d"),HttpResponse->GetResponseCode());
		}
	});
	httpRequest->ProcessRequest();
}
