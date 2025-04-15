#pragma once

#include "CoreMinimal.h"
#include "Itemdata.generated.h"

USTRUCT(BlueprintType)
struct FItemData: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name = FString("Name");
};