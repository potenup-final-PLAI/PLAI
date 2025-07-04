﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestOrderActor.h"

#include "NiagaraComponent.h"


// Sets default values
AQuestOrderActor::AQuestOrderActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NiagaraCompQuest = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
	NiagaraCompQuest->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AQuestOrderActor::BeginPlay()
{
	Super::BeginPlay();

	// NiagaraCompQuest->SetActive(false);
}

// Called every frame
void AQuestOrderActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

