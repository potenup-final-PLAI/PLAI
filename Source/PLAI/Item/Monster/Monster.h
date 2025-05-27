// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterStruct.h"
#include "GameFramework/Character.h"
#include "Monster.generated.h"


UCLASS()
class PLAI_API AMonster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMonster();

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
	class UMonFsm* MonFsm;
	
	UPROPERTY(EditAnywhere)
	class ATestPlayer* TestPlayer;
	
	// UPROPERTY(EditAnywhere)
	// class USphereComponent* SphereComponent;

    UPROPERTY(EditAnywhere)
	class UWidgetComponent* MonUiComp;

    UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<class AMonster> MonsterFactory;
	// 이건 부모에서는 절대 쓰지말아야할 함수임 !!
	UPROPERTY(EditAnywhere)
	class AMonster* MonsterParent;
	
    UPROPERTY(EditAnywhere)
	TSubclassOf<class UMonUi> MonUiFactory;

	UPROPERTY(EditAnywhere)
	class UMonUi* MonUi;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMonDamage> MonDamageFactory;

    UPROPERTY(EditAnywhere)
	class UMonDamage* MonDamage;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AItemMaster> ItemMasterFactory;
	
	UPROPERTY(EditAnywhere)
	FMonsterStruct MonsterStruct;

	UPROPERTY(EditAnywhere)
	UDataTable* MonsterTable;

	//임시 코드임 !!
	UPROPERTY(EditAnywhere)
	TArray<FDataTableRowHandle> DataTableRows;

	virtual void SetMonsterUi();
	virtual void SetHpBar();
	void Dead();

	UPROPERTY(EditAnywhere)
	FTimerHandle DamageUiTimerHandle;
	
	UFUNCTION()
	void DamageUi(float Damage);

	void DamageUiDestroy(UWidgetComponent* DamageComp);
};
