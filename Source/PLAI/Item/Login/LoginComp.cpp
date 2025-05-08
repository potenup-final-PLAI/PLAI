// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginComp.h"
#include "HttpModule.h"
#include "IWebSocket.h"
#include "JsonObjectConverter.h"
#include "LogItemComp.h"
#include "UserStruct.h"
#include "WebSocketsModule.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WrapBox.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "PLAI/Item/GameInstance/WorldGi.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/Npc/NpcNet.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/TestPlayer/TraitStructTable/TraitStructTable.h"
#include "PLAI/Item/UI/Inventory/EquipInven/EquipInven.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemInven.h"
#include "PLAI/Item/UI/Main/UIChaMain.h"
#include "PLAI/Item/UI/Main/UiChaView.h"
#include "PLAI/Item/UI/Main/UIinitMain.h"
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
	
    if (FModuleManager::Get().IsModuleLoaded("WebSocket"))
    {
    	FModuleManager::Get().LoadModule("WebSocket");
    }
	
    TestPlayer = Cast<ATestPlayer>(GetOwner());
	
	if (TestPlayer->IsLocallyControlled())
	{
		if (UWorldGi* WorldGi = Cast<UWorldGi>(GetWorld()->GetGameInstance()))
		{
			if (WorldGi->bGameStart == false)
			{
				UiMain = CreateWidget<UUiMain>(GetWorld(),UiMainFactory);
				UiMain->AddToViewport();
				UiMain->LoginComp = this;
				UiMain->WbpUiSign->LoginComp = this;
			}
		}
	}
}


// Called every frame
void ULoginComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// DrawDebugString(GetWorld(),TestPlayer->GetActorLocation() + FVector(0, 0, 100),
	// FString::Printf(TEXT("LoginComp 나의 UserId [%s] \n "
	// 				              "LoginComp 나의 CharacterId [%s]"),*UserFullInfo.user_id, *UserFullInfo.character_info.character_id),
	// 				  nullptr,FColor::Red,0.f,false);

	if (APlayerController* PC = Cast<APlayerController>(TestPlayer->GetController()))
	{ if (PC->WasInputKeyJustPressed(EKeys::One)) // 1 캐릭터 생성 요청
	{   UE_LOG(LogTemp,Display,TEXT("로그인 컴프 1키 User 구조체 UserId 정보조회 %s"),*UserFullInfo.user_id);
		UE_LOG(LogTemp,Display,TEXT("로그인 컴프 1키 User Character Id 정보조회 %s"),*character_id);
	}
	}
	
	// if (APlayerController* PC = Cast<APlayerController>(TestPlayer->GetController()))
	// { if (PC->WasInputKeyJustPressed(EKeys::C)) // C 캐릭터 생성 요청
	// 	{   UE_LOG(LogTemp,Display,TEXT("Input C 캐릭터 생성 요청 Key JustPressed"));
	// 	    HttpCreatePost();
	// 	}
	// }

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
	{ if (PC->WasInputKeyJustPressed(EKeys::B)) // B 웹소켓연걸
	{
		ConnectWebSocket();
		UE_LOG(LogTemp,Display,TEXT("Input B 웹소켓 연결 Key JustPressed"));
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
		    		// UE_LOG(LogTemp, Error, TEXT("LoginComp NpcNet 없네"));
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
    FNgrok Ngrok;
	
	FHttpRequestRef httpRequest = FHttpModule::Get().CreateRequest();

	httpRequest->SetURL(Ngrok.Ngrok + TEXT("/users/login"));
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
		FLoginStructGet LoginStructGetInit;
		FLoginStructGet LoginStructGet;
		if (bProcessedSuccessfully)
		{ FString JsonString = HttpResponse->GetContentAsString();
			UE_LOG(LogTemp, Warning, TEXT("로그인컴프 통신성공 로그인 JsonString무엇 [%s]"), *JsonString);

			FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &LoginStructGet);
			if (LoginStructGetInit.user_id != LoginStructGet.user_id)
			{ OnLogin.ExecuteIfBound(true);
				UE_LOG(LogTemp, Warning, TEXT("로그인컴프 통신성공 로그인%s"),*LoginStructGet.user_id);
				UserFullInfo.user_id = LoginStructGet.user_id;
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
	FNgrok Ngrok;
    FString url = FString::Printf(TEXT("%s/users/register"),*Ngrok.Ngrok);
	httpRequest->SetURL(url);
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
			UE_LOG(LogTemp,Warning,TEXT("로그인컴프 가입요청 성공 %s 답변코드 %d"),*JsonString,HttpResponse->GetResponseCode());
			if (UiMain->WbpUiSign)
			{
				if (HttpResponse->GetResponseCode() == 200)
				{
					UiMain->WbpUiSign->SignCompleteBox->SetVisibility(ESlateVisibility::Visible);
					UiMain->WbpUiSign->bSingComplete->SetText(FText::FromString(TEXT("가입 완료")));
				}
				else
				{
					UiMain->WbpUiSign->SignCompleteBox->SetVisibility(ESlateVisibility::Visible);
					UiMain->WbpUiSign->bSingComplete->SetText(FText::FromString(TEXT("가입 실패")));
					UiMain->WbpUiSign->bSingCompleteDetail->SetText(FText::FromString(JsonString));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("로그인컴프 가입요청 실패 %d"),HttpResponse->GetResponseCode());
		}
	});
	httpRequest->ProcessRequest();
}

void ULoginComp::Server_HttpMePost_Implementation()
{
	Client_HttpMePost();
}

void ULoginComp::Client_HttpMePost_Implementation()
{
	HttpMePost();
}

void ULoginComp::HttpMePost()
{
	FHttpRequestRef httpRequest = FHttpModule::Get().CreateRequest(); FNgrok Ngrok;
	FString url = FString::Printf(TEXT("%s/me/"),*Ngrok.Ngrok);

	httpRequest->SetURL(url);
	httpRequest->SetVerb("POST");
	httpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	FString JsonString;
	FMeStruct MeStruct;
	
	MeStruct.user_id = UserFullInfo.user_id;
	
	FJsonObjectConverter::UStructToJsonObjectString(MeStruct,JsonString);
	httpRequest->SetContentAsString(JsonString);
	httpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bProcessedSuccessfully)
	{
		if (bProcessedSuccessfully)
		{
			FString JsonString = HttpResponse->GetContentAsString();
			UE_LOG(LogTemp,Warning,TEXT("로그인컴프 나의정보 GetAsContetAsString된 제이슨값 성공 %s"),*JsonString);
			
			if (JsonString.IsEmpty())
			{
				UE_LOG(LogTemp, Error, TEXT("Login Comp // 서버 응답은 성공했지만 내용이 비었습니다."));
				return;
			}
			// 2. JSON → Struct 파싱 시도
			if (!FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &UserFullInfo))
			{
				UE_LOG(LogTemp, Error, TEXT("Login Comp // JSON 파싱 실패: 구조체 변환에 실패했습니다."));
				return;
			}
			// 3. 파싱 성공했더라도, 필수 값이 들어있는지 유효성 검사
			if (UserFullInfo.character_info.character_id.IsEmpty())
			{
				UE_LOG(LogTemp, Error, TEXT("Login Comp // UserFullInfo 필수 값이 비어 있음 (character_id 없음)."));
				return;
			}
			
			UE_LOG(LogTemp,Warning,TEXT("로그인컴프 나의정보 조회 성공 %s"),*JsonString);
			FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &UserFullInfo);
			FString GetJson;
			
			FJsonObjectConverter::UStructToJsonObjectString(UserFullInfo,GetJson);
			UE_LOG(LogTemp,Warning,TEXT("로그인컴프 나의정보 조회 Json변환 %s"),*GetJson);

			UiMain->Wbp_UIChaMain->SetUiChaStat(&UserFullInfo);

			LoadEquipItem();
			LoadInvenItem();
			TestPlayer->InvenComp->MenuInven->Wbp_ChaView->NameCha->SetText
			(FText::FromString(UserFullInfo.character_info.character_name));
		}
	});
	httpRequest->ProcessRequest();
}

void ULoginComp::SetTrait()
{
	
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
			    		TestPlayer->InvenComp->Server_EquipItem(*ItemStructTable,SlotEquip->SlotType);
			    		SlotEquip->ItemStructTable = *ItemStructTable;
			    		SlotEquip->SlotImageUpdate();
			    		break;
			    	}
			    }
			}
		}
	}
}

void ULoginComp::LoadInvenItem()
{
	if (!TestPlayer -> IsLocallyControlled()){return;}
	
	TestPlayer->InvenComp->SetGold(UserFullInfo.inventory_info.gold);
	if (USlot* Slot = Cast<USlot>(TestPlayer->InvenComp->MenuInven->WBP_ItemInven->WrapBox->GetChildAt(0)))
	{
		TArray<FName>RawNames = Slot->ItemTable->GetRowNames();
		for (int32 i = 0; i < UserFullInfo.inventory_info.item_list.Num(); i++)
		{
			for (FName RawName : RawNames)
			{
				FItemStructTable* ItemStructTable = Slot->ItemTable->FindRow<FItemStructTable>(RawName,"LoginComp");
				if (ItemStructTable->Item_Id == UserFullInfo.inventory_info.item_list[i].item_id)
				{
					USlot* SlotInvel = Cast<USlot>(TestPlayer->InvenComp->MenuInven->
						WBP_ItemInven->WrapBox->GetChildAt(i));
					SlotInvel->ItemStructTable = *ItemStructTable;
					SlotInvel->SlotImageUpdate();
				}
			}
		}
	}
	
	// for (UWidget* Widget : TestPlayer->InvenComp->MenuInven->WBP_ItemInven->WrapBox->GetAllChildren())
	// {
	// 	if (USlot* SlOT = Cast<USlot>(Widget))
	// 	{
	// 		
	// 	}
	// }
}



void ULoginComp::HttpCreatePost(FString CharacterName)
{
	FHttpRequestRef httpRequest = FHttpModule::Get().CreateRequest();
	FNgrok Ngrok;

	FString Endpoint = FString::Printf(TEXT("%s/characters/create"), *Ngrok.Ngrok);
	httpRequest->SetURL(Endpoint);
	
	httpRequest->SetVerb("POST");
	httpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	FCreateStruct CreateStruct;
	
	CreateStruct.user_id = UserFullInfo.user_id;
	CreateStruct.character_name = CharacterName;

	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(CreateStruct, JsonString);
	UE_LOG(LogTemp,Display,TEXT("로그인컴프 Json String 쏘기전 : %s"),*JsonString);
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

void ULoginComp::ConnectWebSocket()
{
	const FString URL = FString::Printf(TEXT(
		"wss://919e-221-148-189-129.ngrok-free.app/service1/ws/characters/create/%s"),*UserFullInfo.user_id);
	
	UE_LOG(LogTemp,Warning,TEXT("LoginComp WebSocket 연결된 주소[%s]"),*URL)
	const FString Protocol = TEXT("");
	
	WebSocket = FWebSocketsModule::Get().CreateWebSocket(URL, Protocol);
	// 연결 성공
	WebSocket->OnConnected().AddUObject(this, &ULoginComp::OnWebSocketConnected);
	// 메시지 수신
	WebSocket->OnMessage().AddUObject(this, &ULoginComp::OnWebSocketMessage);
	// 에러 처리
	WebSocket->OnConnectionError().AddUObject(this, &ULoginComp::OnWebSocketConnectionError);
	// 종료 처리
	WebSocket->OnClosed().AddUObject(this, &ULoginComp::OnWebSocketClosed);
	// 실제 연결 시도
	WebSocket->Connect();
}

void ULoginComp::OnWebSocketConnected()
{
	UE_LOG(LogTemp, Log, TEXT("✅ LoginComp WebSocket Connected!"));
}

void ULoginComp::OnWebSocketMessage(const FString& Msg)
{
	UE_LOG(LogTemp, Warning, TEXT("LoginComp 웹소켓 메시지 [%s]"), *Msg);
	UiMain->Wbp_UiInitMain->InitResponse->SetText(FText::FromString(Msg));
}

void ULoginComp::OnWebSocketConnectionError(const FString& Error)
{
	UE_LOG(LogTemp, Warning, TEXT("LoginComp 웹소켓 에러 [%s]"), *Error);
}

void ULoginComp::SendInitStringWebSocket(const FString& Message)
{
	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		WebSocket->Send(Message);
		UE_LOG(LogTemp, Log, TEXT("▶ Sent via WS: %s"), *Message);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WebSocket not connected!"));
	}
}

void ULoginComp::OnWebSocketClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	UE_LOG(LogTemp,Warning,TEXT("🔒LoginCOmp 웹소켓 Closed: Code=%d Reason=%s Clean=%d"), StatusCode, *Reason, bWasClean);
}
