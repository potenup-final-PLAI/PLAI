// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameFramework/Actor.h"
#include "QuestOrderActor.generated.h"

enum class EQuestType : uint8;

UCLASS()
class PLAI_API AQuestOrderActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AQuestOrderActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	EQuestType QuestType;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* NiagaraCompQuest;
};
