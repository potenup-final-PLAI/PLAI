// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NpcCharacter.h"
#include "GameFramework/Character.h"
#include "PLAI/Item/UI/Net/UiPost.h"
#include "NpcNet.generated.h"

UCLASS()
class PLAI_API ANpcNet : public ANpcCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANpcNet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUiPost>UIPostFactory;
	UPROPERTY(EditAnywhere)
	class UUiPost* UIPost;

	UPROPERTY(EditAnywhere)
	APlayerController* pc;

	UPROPERTY(EditAnywhere)
	FString personality = FString(TEXT("겁에질린"));
	
	void OpenQuest();
	void NetPost(FString String);
};
