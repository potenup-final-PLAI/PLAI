// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BattlePlayerAnimInstance.h"

#include "Battle/Util/DebugHeader.h"
#include "Enemy/BaseEnemy.h"
#include "Net/UnrealNetwork.h"
#include "Player/BattlePlayer.h"

void UBattlePlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	battlePlayer = Cast<ABattlePlayer>(GetOwningActor());
}

void UBattlePlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);


	// if (auto* player = Cast<ABaseBattlePawn>(TryGetPawnOwner()))
	// {
	// 	// 전방방향 벡터 구하기
	// 	FVector velocity = player->GetVelocity();
	// 	FVector forward = player->GetActorForwardVector();
	// 	moveSpeed = FVector::DotProduct(velocity, forward);
	// }
}

void UBattlePlayerAnimInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UBattlePlayerAnimInstance, actionMode);
	DOREPLIFETIME(UBattlePlayerAnimInstance, lifeState);
	DOREPLIFETIME(UBattlePlayerAnimInstance, moveSpeed);
	DOREPLIFETIME(UBattlePlayerAnimInstance, battlePlayer);
}

void UBattlePlayerAnimInstance::AnimNotify_BaseAttackPoint()
{
	// battlePlayer나 targetEnemy가 Null이면 return
	if (!(battlePlayer && battlePlayer->targetEnemy))
	{
		UE_LOG(LogTemp, Warning, TEXT("battlePlayer Or targetEnemy Nullptr"));
		return;
	}
	NET_PRINTLOG(TEXT("AnimNotify_BaseAttackPoint"));
	if (battlePlayer->HasAuthority())
	{
		battlePlayer->PlayerApplyAttack(battlePlayer->targetEnemy, EActionMode::BaseAttack);
	}
	else if (!battlePlayer->HasAuthority() && battlePlayer->IsLocallyControlled())
	{
		battlePlayer->Server_PlayerApplyAttack(battlePlayer->targetEnemy, EActionMode::BaseAttack);
	}

	battlePlayer->targetEnemy = nullptr;
	battlePlayer->attackTarget = nullptr;
}
