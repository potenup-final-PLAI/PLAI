// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PLAI/Item/UI/Npc/UiNpc.h"
#include "NpcCharacter.generated.h"

class ATestPlayer;

UCLASS()
class PLAI_API ANpcCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANpcCharacter();

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
	class TSubclassOf<UUiNpc>UiNpcFactory;
	UPROPERTY(EditAnywhere)
	class UUiNpc* UiNpc;

	UPROPERTY(EditAnywhere)
	class UWidget* NpcUiMaster;

	UPROPERTY(EditAnywhere)
	FString NpcNameString = TEXT("초기 NPC");

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UWidgetComponent* WidgetComp;

	UPROPERTY(EditAnywhere)
	class ATestPlayer* TestPlayer;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* NiagaraComp;
};
