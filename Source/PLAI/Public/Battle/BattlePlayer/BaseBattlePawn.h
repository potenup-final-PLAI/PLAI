// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "BaseBattlePawn.generated.h"

UCLASS()
class PLAI_API ABaseBattlePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseBattlePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	//----------Speed State-------------
	int32 speed = 0;
	//------------Turn System-----------------
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ATurnManager> turnManagerFactory;
	
	void OnTurnStart();
	void OnTurnEnd();

	//------------Skill System-----------------
	
	//------------Move System-----------------
	// 이 변수에 들어있는 Block 들을 기준으로 상, 하, 좌, 우 검색 후 Cost 구해야 한다.
	UPROPERTY(EditAnywhere)
	TArray<class AGridTile*> openArray;
	// 검색이 다 끝난 타일
	UPROPERTY(EditAnywhere)
	TArray<class AGridTile*> closedArray;
	// 시작 타일
	UPROPERTY(EditAnywhere)
	class AGridTile* startTile;
	// 목적지 타일
	UPROPERTY(EditAnywhere)
	class AGridTile* goalTile;
	// 현재 체크하고 있는 block
	UPROPERTY(EditAnywhere)
	class AGridTile* currentTile;

	UPROPERTY(EditAnywhere)
	class UInputMappingContext* IMC_Player;
	UPROPERTY(EditAnywhere)
	class UInputAction* IA_Move;
	// 움직임 플래그
	bool bIsMoveMode = false;
	
	// mouse Click
	void MouseClick(const FInputActionValue& value);
	// UI에서 이동을 눌렀을 때 범위 활성화
	void PathFind();
	void BuildPath();
	void AddOpenArray(FVector dir);
	// 골 위치를 클릭 했을 때 그쪽으로 이동
	void UnitMove();
	
};
