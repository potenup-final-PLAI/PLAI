// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(BattleNetLog, Log, All)

#define LOCALROLE (UEnum::GetValueAsString<ENetRole>(GetLocalRole()))
#define REMOTEROLE (UEnum::GetValueAsString<ENetRole>(GetRemoteRole()))

#define NETMODE (GetWorld()->GetNetMode()==ENetMode::NM_Client ? TEXT("Client") : GetWorld()->GetNetMode()==NM_Standalone ? TEXT("Standalone") : TEXT("Server"))

#define CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define NET_PRINTLOG(fmt, ...) UE_LOG(BattleNetLog, Error, TEXT("[%s]%s : %s"), NETMODE, *CALLINFO, *FString::Printf(fmt, ##__VA_ARGS__));