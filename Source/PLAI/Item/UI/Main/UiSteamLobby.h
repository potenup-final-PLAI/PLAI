// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UISteamLobbyScrollRoom/UISteamLobbyScrollRoom.h"
#include "UiSteamLobby.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUiSteamLobby : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUISteamLobbyScrollRoom> ScrollRoomFactory;
	
	UPROPERTY(EditAnywhere)
	class UUISteamLobbyScrollRoom* SteamLobbyScrollRoom;

	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ScrollBox;
	void AddScrollBox(FString SessionName);
	
	UPROPERTY(EditAnywhere)
	class UUiMain* UiMain;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button_CreateRoom;
	UFUNCTION()
	void OnCreateRoom();
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_FindRoom;
	UFUNCTION()
	void OnFindRoom();

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* RoomName;

	
	// UFUNCTION()
	// void OnFindRoom();
	
	// UPROPERTY(meta = (BindWidget))
 //    class UButton* Button_JoinRoom;
 //    UFUNCTION()
 //    void OnJoinRoom();

	virtual void NativeConstruct() override;
};
