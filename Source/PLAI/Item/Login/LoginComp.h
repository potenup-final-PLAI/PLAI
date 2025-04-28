// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "UserStruct.h"

DECLARE_DELEGATE_OneParam(FOnLogin, bool bLogin)
DECLARE_DELEGATE_OneParam(FOnSing, bool bSign)

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PLAI/Item/UI/Main/UiMain.h" 
#include "LoginComp.generated.h"

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
	FUserFullInfo UserFullInfo;
	
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

	// UPROPERTY(EditAnywhere)
	// TSubclassOf<class UMain> MainFactory;
	//
	// UPROPERTY(EditAnywhere)
	// class UMain* Main;

	UPROPERTY(EditAnywhere)
	class ATestPlayer* TestPlayer;
	
	void HttpSignPost();

	void HttpLoginPost();
	
	void HttpCreatePost();
	
	void HttpMePost();

	void LoadEquipItem();
	//테스트 테이블 변환
	// void TransDataTable();

	bool bQuest = true;
	
};
