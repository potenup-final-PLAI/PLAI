// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

DECLARE_DELEGATE_OneParam(FOnLogin, bool bLogin)
DECLARE_DELEGATE_OneParam(FOnSing, bool bSign)

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/IHttpRequest.h"
#include "PLAI/Item/UI/Main/UiMain.h" 
#include "LoginComp.generated.h"

// 서버 가입 요청 토큰
USTRUCT()
struct FLoginStructGet
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
	FString email = TEXT("Email");
	
	UPROPERTY(EditAnywhere)
	FString password = TEXT("PassWord");

	UPROPERTY(EditAnywhere)
	bool bInitCharacter = false;
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

	UPROPERTY(EditAnywhere)
	FString UserId;
	UPROPERTY(EditAnywhere)
	FString UserPw;

	void SaveEquip();

	void HttpLoginPost();
	
	void HttpSignPost();

	void HttpInitPost();

	//테스트 테이블 변환
	void TransDataTable();
	
};
