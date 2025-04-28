#pragma once

#include "CoreMinimal.h"
#include "ItemStructPost.generated.h"


USTRUCT(BlueprintType,Blueprintable)
struct FItemStructPost: public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere) //RawName 키값; 
	FString item_id = FString("ItemId");
	UPROPERTY(EditAnywhere) //아이템 형태(장비/소비/기타) ;
	int32 item_category = 0;
	UPROPERTY(EditAnywhere) //아이템 종류(무기/갑옷/투구);
	int32 item_type = 0;
	UPROPERTY(EditAnywhere) //아이템 직업(전사/궁수)
	int32 item_class = 0;
	UPROPERTY(EditAnywhere) //아이템 이름;
	FString item_name = FString("Name"); 
	UPROPERTY(EditAnywhere) //아이템 종류;
	FString category_name = FString("Type");
	UPROPERTY(EditAnywhere) //아이템 설명;
	FString description = FString("Detail");

	// 갯수는??
	UPROPERTY(EditAnywhere) //아이템 갯수 필요할듯???;
	int32 ItemNum = 0;
	UPROPERTY(EditAnywhere) //아이템 판매 가격;
	int32 price = 0;
	UPROPERTY(EditAnywhere) //아이템 착용 레벨
	int32 level = 0;
};
