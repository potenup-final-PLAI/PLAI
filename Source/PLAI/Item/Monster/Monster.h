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
	class USphereComponent* SphereComponent;

    UPROPERTY(EditAnywhere)
	class UWidgetComponent* MonUiComp;

    UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<class AMonster> MonsterFactory;
	
	UPROPERTY(EditAnywhere)
	class AMonster* MonsterParent;
	
    UPROPERTY(EditAnywhere)
	TSubclassOf<class UMonUi> MonUiFactory;
	
	UPROPERTY(EditAnywhere)
	class UMonUi* MonUi;
	
	UPROPERTY(EditAnywhere)
	FMonsterStruct MonsterStruct;

	//임시 코드임 !!
	UPROPERTY(EditAnywhere)
	TArray<FDataTableRowHandle> DataTableRows;

	void SetMonsterUi();

	void SetHpBar();
};
