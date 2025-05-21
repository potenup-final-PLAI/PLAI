// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "UserStruct.h"

DECLARE_DELEGATE_OneParam(FOnLogin, bool bLogin)
DECLARE_DELEGATE_OneParam(FOnSing, bool bSign)

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "IWebSocket.h"  

#include "PLAI/Item/UI/Main/UiMain.h" 
#include "LoginComp.generated.h"

USTRUCT(BlueprintType)
struct FNgrok
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FString Ngrok = FString("https://0b55-221-148-189-129.ngrok-free.app/service1");
};

// 서버 가입 요청 토큰
USTRUCT(BlueprintType)
struct FLoginStructRes
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FString access_token = TEXT("string");
	UPROPERTY()
	FString refresh_token = TEXT("string");
	UPROPERTY()
	FString token_type = TEXT("string");
	UPROPERTY()
	FString user_id = TEXT("string");
};

// 서버 로그인 요청 토큰
USTRUCT(BlueprintType)
struct FLoginStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FString email = TEXT("string");
	UPROPERTY(EditAnywhere)
	FString password = TEXT("string");
};

// 서버 로그인 답변 토큰
USTRUCT(BlueprintType)
struct FLoginStructGet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	FString massage = TEXT("Email");
	UPROPERTY(EditAnywhere)
	FString access_token = TEXT("Email");
	UPROPERTY(EditAnywhere)
	FString refresh_token = TEXT("PassWord");
	UPROPERTY(EditAnywhere)
	FString token_type = TEXT("Email");
	UPROPERTY(EditAnywhere)
	FString user_id = TEXT("Email");
};

USTRUCT(BlueprintType)
struct FCreateStruct
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	FString user_id = TEXT("Email");
	UPROPERTY(EditAnywhere)
	FString character_name = TEXT("Char_non");
	UPROPERTY(EditAnywhere)
	FString job = TEXT("warrior");
	UPROPERTY(EditAnywhere)
	FString gender = TEXT("M");
	UPROPERTY(editanywhere)
	TArray<FString> Traits;
};

USTRUCT(BlueprintType)
struct FCreateStructGet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	FString massage = TEXT("Email");
	UPROPERTY(EditAnywhere)
	FString character_id = TEXT("Cha None");
};

USTRUCT(BlueprintType)
struct FMeStruct
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	FString user_id = TEXT("Email");
};

USTRUCT(BlueprintType)
struct FMeStructGet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	FString user_id = TEXT("Email");
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAI_API ULoginComp : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULoginComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditAnywhere)
	FUserFullInfo UserFullInfoStat;

	UPROPERTY(EditAnywhere)
	FUserFullInfo UserFullInfo;

	UPROPERTY(EditAnywhere)
	UDataTable* TraitStructTable;

	UPROPERTY(EditAnywhere)
	UDataTable* LevelTable;
	
    UPROPERTY(EditAnywhere)
	FString User_id = FString("user_id");
	
	UPROPERTY(EditAnywhere)
	FString character_id = FString("");
	
	UPROPERTY(EditAnywhere)
	FName OldLevelStreamName;
	
	UPROPERTY(EditAnywhere)
    FName NewLevelStreamName;
	
	FOnLogin OnLogin;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUiMain> UiMainFactory;
	
	UPROPERTY(EditAnywhere)
	class UUiMain* UiMain;

	UPROPERTY(EditAnywhere)
	class ATestPlayer* TestPlayer;
	
	void HttpSignPost();

	void HttpLoginPost();
	
	void HttpCreatePost(FString CharacterName);

	UFUNCTION(Server, Reliable)
    void Server_HttpMePost();
	UFUNCTION(Client, Reliable)
	void Client_HttpMePost();
	
	void HttpMePost();
	
	void LoadEquipItem();
	void LoadInvenItem();

	/** 런타임에 받은 토큰을 넣어서 WS 연결 */
	void ConnectWebSocket();

	/** WS 인스턴스 보관 */
	TSharedPtr<IWebSocket> WebSocket;
	
	/** 콜백들 */
	
	void OnWebSocketConnected();
	void OnWebSocketMessage(const FString& Msg);
	void OnWebSocketConnectionError(const FString& Error);
	void SendInitStringWebSocket(const FString& Message);
	void OnWebSocketClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
	
	//테스트 테이블 변환
	// void TransDataTable();

	bool bQuest = true;
};
