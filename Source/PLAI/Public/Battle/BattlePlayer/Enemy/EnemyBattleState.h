// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/Util/BattleType/BattleTypes.h"
#include "UObject/Object.h"
#include "EnemyBattleState.generated.h"

/**
 * 
 */
UCLASS()
class PLAI_API UEnemyBattleState : public UObject
{
	GENERATED_BODY()

public:
	// Enemy State 부분
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	int32 hp = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	int32 attack = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	int32 defense = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	int32 resistance = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	int32 move_Range = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float critical_Rate = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	float critical_Damage = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	int32 speed = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	TArray<FString> traits = {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	TArray<FString> skills = {};
	//--------------------Status API 전송용 구조체 변수--------------------
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Battle")
	FBaseStatus enemyStatus;

	//--------------------LifeState--------------------
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Battle")
	ELifeState enemyLifeState;
	
	// Enemy AP 부분
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Phase")
	int32 maxActionPoints = 5;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Phase")
	int32 curAP = 0;


	// Player AP 세팅
	void SetAP()
	{
		curAP = 0;
	}

	// AP 증가 시키는 함수
	int32 GetAP(int32 ap)
	{
		if (ap >= maxActionPoints)
		{
			return ap;
		}
		// AP 증가시켜서 리턴
		return ++ap;
	}

	// 스킬이 사용한지 체크하는 함수
	bool CanUseSkill(int32 cost)
	{
		// 호출하는 대상의 AP를 검사해서 크다면 스킬 실행
		return curAP >= cost;
	}
	// AP 소비하는 함수
	bool CanConsumeAP(int32 cost)
	{
		if (!CanUseSkill(cost))
		{
			UE_LOG(LogTemp, Warning, TEXT("Not enough AP"));
			return false;
		}
		// 호출하는 대상에 ap를 받아서 AP 증감처리 
		curAP = FMath::Max(0, curAP - cost);
		// 처리된 ap를 다시 호출한 대상 AP에 업데이트
		return true;
	}
	
	
};
