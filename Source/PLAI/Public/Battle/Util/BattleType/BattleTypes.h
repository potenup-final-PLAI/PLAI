// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BattleTypes.generated.h"

//-------Battle Start 할 때 넘겨줘야 할 캐릭터 데이터 구조체-------
USTRUCT(BlueprintType)
struct FCharacterData
{
	GENERATED_BODY()
	// 플레이어 / 몬스터 ID 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString id = "";
	// 플레이어 / 몬스터 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString name = "";
	// 플레이어인지 몬스터인지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString type = "";
	// 플레이어 / 몬스터 성격
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> traits = {};
	// 플레이어 / 몬스터 스킬
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> skills = {};
};
// 캐릭터 정보들과 지역, 날씨 정보용 구조체 
USTRUCT(BlueprintType)
struct FEnvironmentState
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCharacterData> characters = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString terrain = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString weather = "";
};

//-------Action API 보낼 데이터 구조체-------
USTRUCT(BlueprintType)
struct FCharacterStatus
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString id = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 hp = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ap = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 mov = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> status_effects = {};
};
USTRUCT(BlueprintType)
struct FBattleTurnState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 cycle = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 turn = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString current_character_id = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCharacterStatus> characters = {};
};

//-------Action API Response 받은 데이터--------
USTRUCT(BlueprintType)
struct FBattleAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> move_to = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString skill = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString target_character_id = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString reason = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString dialogue = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 remaining_ap = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 remaining_mov = 0;
};
USTRUCT(BlueprintType)
struct FActionRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString current_character_id = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBattleAction action = {};
};

// 캐릭터 스텟
USTRUCT(BlueprintType)
struct FBaseStatus
{
	GENERATED_BODY()

	UPROPERTY()
	int32 hp = 0;
	UPROPERTY()
	int32 attack = 0;
	UPROPERTY()
	int32 defense = 0;
	UPROPERTY()
	int32 resistance = 0;
	UPROPERTY()
	float critical_Rate = 0.0f;
	UPROPERTY()
	float critical_Damage = 0.0f;
	UPROPERTY()
	int32 move_Range = 0;
	UPROPERTY()
	int32 speed = 0;
	UPROPERTY()
	int32 points = 0;
	UPROPERTY()
	TArray<FString> traits = {};
	UPROPERTY()
	TArray<FString> skills = {};
};
UENUM(BlueprintType)
enum class ELifeState : uint8
{
	Alive UMETA(DisplayName = "Alive"),
	Dead UMETA(DisplayName = "Dead"),
};

UENUM(BlueprintType)
enum class EActionMode : uint8
{
	None UMETA(DisplayName = "None"),
	TurnEnd UMETA(DisplayName = "TurnEnd"),
	Move UMETA(DisplayName = "Move"),
	BaseAttack UMETA(DisplayName = "BaseAttack"),
	Paralysis UMETA(DisplayName = "Paralysis"),
	Poison UMETA(DisplayName = "Poison"),
	Vulnerable UMETA(DisplayName = "Vulnerable"),
	Weakening UMETA(DisplayName = "Weakening"),
	Fatal UMETA(DisplayName = "Fatal"),
	Rupture UMETA(DisplayName = "Rupture"),
	Roar UMETA(DisplayName = "Roar"),
	BattleCry UMETA(DisplayName = "BattleCry"),
	ReadyForBattle UMETA(DisplayName = "ReadyForBattle"),
	Charge UMETA(DisplayName = "Charge"),
	Unique_EarthBreak UMETA(DisplayName = "Unique_EarthBreak"),
	Unique_PoisonSting UMETA(DisplayName = "Unique_PoisonSting"),
	Unique_Harden UMETA(DisplayName = "Unique_Harden"),
	Unique_Instinct UMETA(DisplayName = "Unique_Instinct"),
	Unique_Flexibility UMETA(DisplayName = "Unique_Flexibility"),
	Unique_DefenseOrder UMETA(DisplayName = "Unique_DefenseOrder"),
	Unique_Enrage UMETA(DisplayName = "Unique_Enrage")
};
UENUM(BlueprintType)
enum class EStatusEffect : uint8
{
	Poison UMETA(DisplayName = "Poison"),
	Vulnerable UMETA(DisplayName = "Vulnerable"),
	Weakening UMETA(DisplayName = "Weakening"),
	Angry UMETA(DisplayName = "Angry"),
	Bleeding UMETA(DisplayName = "Bleeding")
};
/**
 * 
 */
UCLASS()
class PLAI_API UBattleTypes : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};
