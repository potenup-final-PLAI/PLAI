// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemStruct.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

DECLARE_DELEGATE_OneParam(FOnItemOverlapped, const FItemStruct&)

UCLASS()
class PLAI_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:

public:
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	FItemStruct ItemStruct;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemStructTop ItemStructTop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AItem> ItemFactory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AItem* ItemParent;
	
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMesh;
	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxComp;

	UPROPERTY(ReplicatedUsing = OnRep_ItemScale)
	FVector ItemScale = FVector(1.0f, 1.0f, 1.0f);

	UFUNCTION()
	void OnRep_ItemScale();

	UFUNCTION()
	void OnMyBeginOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent*
		OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void SetMesh();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
