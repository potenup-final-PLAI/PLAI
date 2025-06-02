// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BattleEnemyAnimInstance.h"

#include "Battle/Util/DebugHeader.h"
#include "Battle/Util/BattleType/BattleTypes.h"
#include "Enemy/BaseEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/BattlePlayer.h"

void UBattleEnemyAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	battleEnemy = Cast<ABaseEnemy>(GetOwningActor());
}

void UBattleEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
}

void UBattleEnemyAnimInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBattleEnemyAnimInstance, lifeState);
	DOREPLIFETIME(UBattleEnemyAnimInstance, actionMode);
	DOREPLIFETIME(UBattleEnemyAnimInstance, moveSpeed);
}

void UBattleEnemyAnimInstance::AnimNotify_BaseAttackPoint()
{
	// battlePlayer나 targetEnemy가 Null이면 return
	if (!(battleEnemy && battleEnemy->targetPlayer))
	{
		UE_LOG(LogTemp, Warning, TEXT("battlePlayer Or targetEnemy Nullptr"));
		return;
	}

	// 사운드 재생
	UGameplayStatics::PlaySoundAtLocation(this, battleEnemy->attackSound, battleEnemy->GetActorLocation());
	// Enemy 대미지 입히기
	battleEnemy->EnemyApplyAttack(battleEnemy->targetPlayer, EActionMode::BaseAttack);
	
	NET_PRINTLOG(TEXT("Enemy %s"), *battleEnemy->GetActorNameOrLabel());
	
	battleEnemy->targetPlayer = nullptr;
	battleEnemy->attackTarget = nullptr;
}
