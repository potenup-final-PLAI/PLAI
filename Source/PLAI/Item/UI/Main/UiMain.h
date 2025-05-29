// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UiMain.generated.h"

USTRUCT(BlueprintType,Blueprintable)
struct FNpcStructPost
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	FString question = FString("Response");
	UPROPERTY(EditAnywhere)
	FString personality = FString("무서운");
};


USTRUCT(BlueprintType,Blueprintable)
struct FNpcStructGet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	FString response = FString("Response");
};

/**
 * 
 */
UCLASS()
class PLAI_API UUiMain : public UUserWidget
{
	GENERATED_BODY()

public:
	//Ui Main
	UPROPERTY(EditAnywhere)
	class ULoginComp* LoginComp;

	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonStart;

	UFUNCTION()
	void OnButtonStart();
	
	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonLogin;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonSignin;

	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonInitPost;

	UPROPERTY(meta = (BindWidget))
	class UUiSign* WbpUiSign;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* LoginId;
	
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* LoginPw;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* loginFailed;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* CanvasMain;

	UPROPERTY(meta = (BindWidget))
	class UUIChaMain* Wbp_UIChaMain;

	UPROPERTY(meta = (BindWidget))
	class UUiSUbMain* Wbp_UiSubMain;

	UPROPERTY(meta = (BindWidget))
	class UUIinitMain* Wbp_UiInitMain;

	UPROPERTY(meta = (BindWidget))
	class UUiSteamLobby* Wbp_UiSteamLobby;
	
	
	
	// Init 클래스
    UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* CanvasInit;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonInitEnd;

	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonCreate;

    UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* CreatePost;
	
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* InitPost;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InitResponse;
	
	UFUNCTION()
	void InitEnd();
	
	UFUNCTION()
	void HttpPostInit();
	
	bool bUiSign = false;
	
	UFUNCTION()
	void SetUiSign();

	UFUNCTION()
	void Login();

	UFUNCTION()
	void CreateCharacter();

	void HttpLoginMe();
	
	
	virtual void NativeConstruct() override;
};
