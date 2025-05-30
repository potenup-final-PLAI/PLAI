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
	UPROPERTY(EditAnywhere)
	class URectLightComponent* firstRectLightComp;
	UPROPERTY(EditAnywhere)
	class URectLightComponent* secondRectLightComp;
	UPROPERTY(EditAnywhere)
	class URectLightComponent* thirdRectLightComp;
	UPROPERTY(EditAnywhere)
	class USpotLightComponent* spotLightComp;
	
	//-------------Input Mapping--------------------
	UPROPERTY(EditAnywhere)
	class UInputMappingContext* IMC_Player;
	UPROPERTY(EditAnywhere)
	class UInputAction* IA_Move;
	
	//------------------Anim Instance--------------------------
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Anim)
	class UBattlePlayerAnimInstance* playerAnim;

	
	UFUNCTION(Server, Reliable)
	void Server_OnClickedTurnEnd();

	UFUNCTION(Server, Reliable)
	void Server_OnClickedMove();

	UFUNCTION(Server, Reliable)
	void Server_OnClickedBaseAttack();

	UFUNCTION(Server, Reliable)
	void Server_OnClickedPoison();

	UFUNCTION(Server, Reliable)
	void Server_OnClickedFatal();

	UFUNCTION(Server, Reliable)
	void Server_OnClickedRupture();


	UFUNCTION(BlueprintCallable)
	void ClearDebugNetLog();

	UFUNCTION(Server, Reliable)
	void Server_UpdatePlayerAnim(EActionMode mode);
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastRPC_UpdatePlayerAnim(EActionMode mode);

	UFUNCTION(Server, Reliable)
	void Server_PlayerTryConsumeAP(int32 amount);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayerTryConsumeAP(int32 amount);
};



