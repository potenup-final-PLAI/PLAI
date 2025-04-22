// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PLAI/Item/UI/Main/UiMain.h" 
#include "LoginComp.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAI_API ULoginComp : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULoginComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUiMain> UiMainFactory;
	
	UPROPERTY(EditAnywhere)
	class UUiMain* UiMain;

	UPROPERTY(EditAnywhere)
	class ATestPlayer* TestPlayer;

	UPROPERTY(EditAnywhere)
	FString UserId;
	UPROPERTY(EditAnywhere)
	FString UserPw;

	void SaveEquip();

	void HttpLogin();
	void HttpSignin();
};
