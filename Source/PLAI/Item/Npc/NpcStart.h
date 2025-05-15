// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NpcCharacter.h"
#include "GameFramework/Character.h"
#include "PLAI/Item/Item/ItemStruct.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/UI/Npc/UiNpcStart.h"
#include "NpcStart.generated.h"

DECLARE_DELEGATE_OneParam(FOnNpcStart, const FItemStructTables)

UCLASS()
class PLAI_API ANpcStart : public ANpcCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANpcStart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// 추후에 다른 곳ㅇ로 옮길 예정
	
	FOnNpcStart OnNpcStart;

    UPROPERTY(EditAnywhere)
	FItemStructTables ItemStructTables;

	UPROPERTY(EditAnywhere)
	UDataTable* ItemTable;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUiNpcStart>UiNpcStartFactory;

	UPROPERTY(EditAnywhere)
	class UUiNpcStart* UiNpcStart;
	
	void HunterStarter();
	
	void WarriorStarter();
};
