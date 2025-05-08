// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PLAI/Item/Item/ItemStruct.h"
#include "PLAI/Item/Monster/MonUi/MonDamage.h"
#include "Creature.generated.h"

USTRUCT(BlueprintType,BlueprintType)
struct FCreStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FString Name = FString("CreName");
	UPROPERTY(EditAnywhere)
	int32 CurrentHp = 100;
	UPROPERTY(EditAnywhere)
	int32 MaxHp = 100;

	UPROPERTY(EditAnywhere)
	int32 CurrentExp = 0;
	UPROPERTY(EditAnywhere)
	int32 MaxExp = 1000;

	UPROPERTY(EditAnywhere)
	int32 Level = 1;
	UPROPERTY(EditAnywhere)
	int32 Atk = 10;
	UPROPERTY(EditAnywhere)
    float Crit = 30.0f;
	UPROPERTY(EditAnywhere)
	float CritDmg = 50.0f;
	
	UPROPERTY(EditAnywhere)
	int32 Def = 10;

	// UPROPERTY(EditAnywhere)
	// FString item_ATK = FString("ATK");
	// UPROPERTY(EditAnywhere)
	// FString Item_CRIT = FString("CRIT");
	// UPROPERTY(EditAnywhere)
	// FString item_CRITDMG = FString("CRITDMG");
	//
	// UPROPERTY(EditAnywhere)
	// FString item_DEF = FString("DEF");
	// UPROPERTY(EditAnywhere)
	// FString item_RES = FString("RES");
	// UPROPERTY(EditAnywhere)
	// FString item_SHI = FString("SHI");
};

UCLASS()
class PLAI_API ACreature : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACreature();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(EditAnywhere)
	class UCreFsm* CreFsm;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMonDamage>MonDamageFactory;

    UPROPERTY(EditAnywhere)
	TSubclassOf<ACreature>CreatureFactory;
	UPROPERTY(EditAnywhere)
	class ACreature* CreatureParent;
	
	UPROPERTY(EditAnywhere)
	class UMonDamage* MonDamage;
	
	UPROPERTY(EditAnywhere)
	FItemStructTable ItemStructTable;
};
