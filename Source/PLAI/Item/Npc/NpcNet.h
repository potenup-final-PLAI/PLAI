// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PLAI/Item/UI/Net/UiPost.h"
#include "NpcNet.generated.h"
USTRUCT(BlueprintType,Blueprintable)
struct FNpcStructNetPost
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FString Post = FString("Post");
	
	UPROPERTY(EditAnywhere)
	FString Get = FString("Get");
};

USTRUCT(BlueprintType,Blueprintable)
struct FNpcStructNetGet
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FString Get = FString("Get");
	
	UPROPERTY(EditAnywhere)
	FString Post = FString("Post");
};


UCLASS()
class PLAI_API ANpcNet : public ACharacter
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
	
	void OpenQuest();
	void NetPost(FString String);
};
