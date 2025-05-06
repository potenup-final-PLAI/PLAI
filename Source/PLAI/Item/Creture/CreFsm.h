// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "PLAI/Item/Monster/MonUi/MonDamage.h"

class AMonster;
class ATestPlayer;
class ACreature;

#include "Creature.h"
#include "CreBullet.h"
#include "Components/ActorComponent.h"
#include "CreFsm.generated.h"

USTRUCT(BlueprintType)
struct FMonsters
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<AMonster*>Monsters;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))

class PLAI_API UCreFsm : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCreFsm();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACreBullet>CreBulletFactory;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FCreStruct CreStruct;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class ATestPlayer* TestPlayer;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class ACreature* Creature;

	void AttackMonster(AMonster* Monster);
	
	void DamageUiDestroy(UWidgetComponent* DamageComp);
	
	void SetCreStat();
	void GetMonGold(AMonster* Monster);
	float PlayerDistance();
	
	FMonsters GetMonsterBySphere(AActor* Actor, float Radios = 2500.0f);
	FVector LineTraceZ(AActor* Actor,FVector Vector);
	float CreatureDamage();
};
