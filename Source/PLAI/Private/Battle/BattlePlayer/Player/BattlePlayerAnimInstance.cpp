// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BattlePlayerAnimInstance.h"

#include "Battle/Util/DebugHeader.h"
#include "Enemy/BaseEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/BattlePlayer.h"
#include "Sound/SoundCue.h"

void UBattlePlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	battlePlayer = Cast<ABattlePlayer>(GetOwningActor());
}

void UBattlePlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
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
	// NET_PRINTLOG(TEXT("AnimNotify_BaseAttackPoint"));
	UGameplayStatics::PlaySoundAtLocation(this, battlePlayer->swordSound, battlePlayer->GetActorLocation());
	
	if (battlePlayer->HasAuthority())
	{
		// NET_PRINTLOG(TEXT("HasAuthority EActionMode::BaseAttack"));
		battlePlayer->PlayerApplyAttack(battlePlayer->targetEnemy, EActionMode::BaseAttack);
	}
	else if (!battlePlayer->HasAuthority() && battlePlayer->IsLocallyControlled())
	{
		// NET_PRINTLOG(TEXT("!battlePlayer->HasAuthority() && battlePlayer->IsLocallyControlled() EActionMode::BaseAttack"));
		battlePlayer->Server_PlayerApplyAttack(battlePlayer->targetEnemy, EActionMode::BaseAttack);
	}

	
	battlePlayer->targetEnemy = nullptr;
	battlePlayer->attackTarget = nullptr;
}

void UBattlePlayerAnimInstance::AnimNotify_PosionAttackPoint()
{
	// battlePlayer나 targetEnemy가 Null이면 return
	if (!(battlePlayer && battlePlayer->targetEnemy))
	{
		UE_LOG(LogTemp, Warning, TEXT("battlePlayer Or targetEnemy Nullptr"));
		return;
	}
	// NET_PRINTLOG(TEXT("AnimNotify_PoisonAttackPoint"));
	UGameplayStatics::PlaySoundAtLocation(this, battlePlayer->swordSound, battlePlayer->GetActorLocation());
	
	if (battlePlayer->HasAuthority())
	{
		// NET_PRINTLOG(TEXT("battlePlayer->HasAuthority() EActionMode::Poison"));
		battlePlayer->PlayerApplyAttack(battlePlayer->targetEnemy, EActionMode::Poison);
	}
	else if (!battlePlayer->HasAuthority() && battlePlayer->IsLocallyControlled())
	{
		// NET_PRINTLOG(TEXT("!battlePlayer->HasAuthority() && battlePlayer->IsLocallyControlled() EActionMode::Poison"));
		battlePlayer->Server_PlayerApplyAttack(battlePlayer->targetEnemy, EActionMode::Poison);
	}

	battlePlayer->targetEnemy = nullptr;
	battlePlayer->attackTarget = nullptr;
}

void UBattlePlayerAnimInstance::AnimNotify_FatalAttackPoint()
{
	// battlePlayer나 targetEnemy가 Null이면 return
	if (!(battlePlayer && battlePlayer->targetEnemy))
	{
		UE_LOG(LogTemp, Warning, TEXT("battlePlayer Or targetEnemy Nullptr"));
		return;
	}
	
	// NET_PRINTLOG(TEXT("AnimNotify_FatalAttackPoint"));
	UGameplayStatics::PlaySoundAtLocation(this, battlePlayer->swordSound, battlePlayer->GetActorLocation());
	
	if (battlePlayer->HasAuthority())
	{
		battlePlayer->PlayerApplyAttack(battlePlayer->targetEnemy, EActionMode::Fatal);
	}
	else if (!battlePlayer->HasAuthority() && battlePlayer->IsLocallyControlled())
	{
		battlePlayer->Server_PlayerApplyAttack(battlePlayer->targetEnemy, EActionMode::Fatal);
	}

	battlePlayer->targetEnemy = nullptr;
	battlePlayer->attackTarget = nullptr;
}

void UBattlePlayerAnimInstance::AnimNotify_RuptureAttackPoint()
{
	// battlePlayer나 targetEnemy가 Null이면 return
	if (!(battlePlayer && battlePlayer->targetEnemy))
	{
		UE_LOG(LogTemp, Warning, TEXT("battlePlayer Or targetEnemy Nullptr"));
		return;
	}
	
	// NET_PRINTLOG(TEXT("AnimNotify_RuptureAttackPoint"));
	UGameplayStatics::PlaySoundAtLocation(this, battlePlayer->ruptureSound, battlePlayer->GetActorLocation());
	
	if (battlePlayer->HasAuthority())
	{
		battlePlayer->PlayerApplyAttack(battlePlayer->targetEnemy, EActionMode::Rupture);
	}
	else if (!battlePlayer->HasAuthority() && battlePlayer->IsLocallyControlled())
	{
		battlePlayer->Server_PlayerApplyAttack(battlePlayer->targetEnemy, EActionMode::Rupture);
	}

	battlePlayer->targetEnemy = nullptr;
	battlePlayer->attackTarget = nullptr;
}
