// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
// #include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "LogItemComp.generated.h"

class ATestPlayer;

USTRUCT(BlueprintType)
struct Foptions
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 hp = 0;
	UPROPERTY(EditAnywhere)
	int32 attack = 0;
	UPROPERTY(EditAnywhere)
	int32 defense = 0;
	UPROPERTY(EditAnywhere)
	int32 resistance = 0;
	UPROPERTY(EditAnywhere)
	int32 critical_rate = 0;
	UPROPERTY(EditAnywhere)
	int32 critical_damage = 0;
	UPROPERTY(EditAnywhere)
	int32 move_range = 0;
	UPROPERTY(EditAnywhere)
	int32 speed = 0;
};

USTRUCT(BlueprintType)
struct Fequipment_info
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FString item_id = TEXT("");
	UPROPERTY(EditAnywhere)
	Foptions options;
};

USTRUCT(BlueprintType)
struct FPostEquipId
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FString character_id = FString("char");
	UPROPERTY(EditAnywhere)
	TArray<Fequipment_info>equipment_info;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAI_API ULogItemComp : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULogItemComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATestPlayer* TestPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APlayerController* PC;
	
	UPROPERTY(EditAnywhere)
	FString Char_id = FString("char");
	
	void HttpEquipPost(FString JsonString);
	
	void GetEquipInfo();

	void GetInvenInfo();
};
