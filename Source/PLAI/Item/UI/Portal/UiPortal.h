// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PLAI/Item/Monster/MonSpawn/MonSpawn.h"
#include "UiPortal.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UUiPortal : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	bool bOpenMap = false;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Village;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Dessert;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Mountain;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button_OpenMap;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Cave;
	
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* WorldMap;

	UPROPERTY(editAnywhere)
	class ATestPlayer* TestPlayer;
	
	UPROPERTY(EditAnywhere)
	class AWarp* Warp;
	
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere)
	TArray<AMonSpawn*> MonSpawns;

	void WarpTestPlayer(EMonSpawnType SpawnType);

	void WarpCotnroller(EMonSpawnType SpawnType);

	UFUNCTION()
	void OnButton_Village();

	UFUNCTION()
	void OnButton_Mountain();
	
	UFUNCTION()
	void OnButton_Dessert();
	
	UFUNCTION()
	void OnButton_Cave();

	UFUNCTION()
	void OnButton_OpenMap();
};
