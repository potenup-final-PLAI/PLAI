// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridTileManager.generated.h"

USTRUCT(BlueprintType)
struct FTileNetData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FString tileName;
	UPROPERTY(EditAnywhere)
	FIntPoint coord;
	UPROPERTY(EditAnywhere)
	class AGridTile* tileInstance;
	
	FTileNetData() : tileName(TEXT("")), coord(FIntPoint(-1, -1)), tileInstance(nullptr) {}
};

UCLASS()
class PLAI_API AGridTileManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGridTileManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Replicated, EditAnywhere)
	class AGridTile* tile;
	UPROPERTY(Replicated, EditAnywhere)
	TSubclassOf<class AGridTile> tileFactory;
	UPROPERTY(EditAnywhere)
	TMap<FIntPoint, class AGridTile*> map;
	UPROPERTY(EditAnywhere)
	TMap<class AGridTile*, FIntPoint> tileToCoordMap;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABattlePlayer> battlePlayerFactory;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABaseEnemy> enemyFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMonBossPawn> bossFactory;

	FTimerHandle bindUnitHandle;

	TArray<class ABattlePlayerController*> playerControllers;

	UFUNCTION(Server, Reliable)
	void ServerRPC_InitGridTile();
	UFUNCTION(Client, Reliable)
	void SendTileData(FIntPoint coord, AGridTile* tilePointer);
	
	TArray<FIntPoint> RandomCoords(int32 count, TArray<FIntPoint> coords);
	//------------Test--------------------------
	// 생성된 유닛들 담기
	TArray<class ABaseBattlePawn*> unitArray;

	// 현재 타일 위치
	AGridTile* FindCurrentTile(FVector worldLoc);
	// 맵에서 x, y위치에 있는 타일 얻기
	AGridTile* GetTile(int32 x, int32 y);
	// 타일 좌표 위치 반환
	FIntPoint GetTileLoc(AGridTile* targetTile);
	// 그 위치에 타일이 있는지 체크
	bool IsValidTile(FIntPoint num);
	// 타일 색 변경
	void SetTileColor(AGridTile* targetTile, bool bHighlight);
	
	UFUNCTION(NetMulticast,Reliable)
	void MulticastRPC_InitClientMap(const TArray<FTileNetData>& tileData);
};
