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

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedItem)
	FItemStructTable ItemStructTable;

	UFUNCTION()
	void OnRep_ReplicatedItem();
	
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

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* NiagaraComp;
	
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* NiagaraSys;

	UFUNCTION()
	void OnMyBeginOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent*
		OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	
	virtual void SetMesh();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
