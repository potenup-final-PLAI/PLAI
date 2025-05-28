#pragma once

#include "CoreMinimal.h"
#include "MonDrop/MonDropTable.h"
#include "MonsterStruct.generated.h"

class AMonster;

USTRUCT(BlueprintType,Blueprintable)
struct FMonsterStruct : public FTableRowBase
{
	GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere)
	int32 MonsterTop = 0;
	
	UPROPERTY(EditAnywhere)
	FString Name = FString("Monster");
	
	UPROPERTY(EditAnywhere)
	int32 Attack = 10;
	
	UPROPERTY(EditAnywhere)
	int32 MaxHp = 100;
	
	UPROPERTY(EditAnywhere)
	int32 CurrentHp = MaxHp;
	
	UPROPERTY(EditAnywhere)
	int32 Exp = 100;
	UPROPERTY(EditAnywhere)
	int32 gold = 100;

	UPROPERTY(EditAnywhere)
	bool bTurn = false; 

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AMonster>>MonsterFactory;

	UPROPERTY(EditAnywhere)
	TArray<FMonDropTable>MonDropTableFactory;
};

class AMonster;

USTRUCT(BlueprintType,Blueprintable)
struct FTurnMonsterStruct : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int32 MonsterTop = 0;
	
	UPROPERTY(EditAnywhere)
	FString Name = FString("Monster");
	
	UPROPERTY(EditAnywhere)
	int32 Attack = 10;
	
	UPROPERTY(EditAnywhere)
	int32 MaxHp = 100;
	
	UPROPERTY(EditAnywhere)
	int32 CurrentHp = MaxHp;
	
	UPROPERTY(EditAnywhere)
	int32 Exp = 100;
	UPROPERTY(EditAnywhere)
	int32 gold = 100;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ATurnMonster>TurnMonster;

	UPROPERTY(EditAnywhere)
	TArray<FMonDropTable>MonDropTableFactory;
};
