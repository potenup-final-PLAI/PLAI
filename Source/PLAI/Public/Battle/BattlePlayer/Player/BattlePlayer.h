// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/BattlePlayer/BaseBattlePawn.h"
#include "BattlePlayer.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API ABattlePlayer : public ABaseBattlePawn
{
	GENERATED_BODY()

public:
	ABattlePlayer();
protected:
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void NotifyControllerChanged() override;

public:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	//-------------Player Settings--------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class USkeletalMeshComponent* meshComp;
	
	//-------------Input Mapping--------------------
	UPROPERTY(EditAnywhere)
	class UInputMappingContext* IMC_Player;
	UPROPERTY(EditAnywhere)
	class UInputAction* IA_Move;
	
	//------------------Anim Instance--------------------------
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Anim)
	class UBattlePlayerAnimInstance* playerAnim;
	
};



