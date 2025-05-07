#pragma once

#include "TraitStructTable.generated.h"

USTRUCT(Blueprintable,BlueprintType)
struct FTraitStructTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FName Trait = FName(TEXT("Trait"));

	UPROPERTY(EditAnywhere)
	FString TraitStat = FString("TraitStat");
	
	UPROPERTY(EditAnywhere)
	int32 Hp = 0;
	UPROPERTY(EditAnywhere)
	int32 Atk = 0;
	UPROPERTY(EditAnywhere)
	int32 Def = 0;
	UPROPERTY(EditAnywhere)
	int32 Crit = 0;
	UPROPERTY(EditAnywhere)
	int32 Mov = 0;
	UPROPERTY(EditAnywhere)
	int32 Spd = 0;
};
