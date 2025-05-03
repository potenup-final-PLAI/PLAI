// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PLAI/Item/Item/ItemStruct.h"
#include "Creature.generated.h"

USTRUCT(BlueprintType,BlueprintType)
struct FCreStruct
{
	GENERATED_BODY()
public:
	FString Name = FString("CreName");
	int32 CurrentHp = 100;
	int32 MaxHp = 100;

	int32 CurrentExp = 0;
	int32 MaxExp = 1000;

	int32 Level = 1;
	int32 Atk = 10;
	int32 Def = 10;
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
	FItemStructTable ItemStructTable;
};
