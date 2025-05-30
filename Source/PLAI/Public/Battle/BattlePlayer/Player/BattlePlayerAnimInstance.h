// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BattlePlayerAnimInstance.generated.h"

enum class ELifeState : uint8;
enum class EActionMode : uint8;
/**
 * 
 */
UCLASS()
class PLAI_API UBattlePlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	// Player가 살아 있는지 죽었는지 판단해서 Animation 업데이트하는 변수
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = Animation)
	ELifeState lifeState;

	// 현재 턴인 Player의 동작 상태를 애니메이션에 업데이트하는 변수
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = Animation)
	EActionMode actionMode;

	//----------Battle Player-----------------
	UPROPERTY(Replicated, BlueprintReadOnly)
	class ABattlePlayer* battlePlayer;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Anim")
	float moveSpeed;

	UFUNCTION(BlueprintImplementableEvent, Category = "Anim")
	void PlayerAttackAnimation(FName sectionName);

	UFUNCTION(BlueprintImplementableEvent, Category = "Anim")
	void PlayHitMotionAnimation(FName sectionName);

	UFUNCTION()
	void AnimNotify_BaseAttackPoint();

	UFUNCTION()
	void AnimNotify_PosionAttackPoint();

	UFUNCTION()
	void AnimNotify_FatalAttackPoint();

	UFUNCTION()
	void AnimNotify_RuptureAttackPoint();
};
