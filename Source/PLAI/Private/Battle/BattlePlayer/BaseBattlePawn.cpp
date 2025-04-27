// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/BattlePlayer/BaseBattlePawn.h"

#include "EnhancedInputSubsystems.h"
#include "GridTile.h"
#include "Battle/TurnSystem/PhaseManager.h"
#include "Battle/TurnSystem/TurnManager.h"
#include "Enemy/BaseEnemy.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Player/BattlePlayer.h"

// Sets default values
ABaseBattlePawn::ABaseBattlePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseBattlePawn::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
}

// Called every frame
void ABaseBattlePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseBattlePawn::OnTurnStart()
{
	if (ABaseEnemy* enemy = Cast<ABaseEnemy>(this))
	{
		FTimerHandle timerHandle;
		GetWorld()->GetTimerManager().SetTimer(timerHandle, FTimerDelegate::CreateLambda([this]()
		{
			OnTurnEnd();
		}), 1.0f, false);
	}
	UE_LOG(LogTemp, Warning, TEXT("%s Turn Start"), *GetName());
}

void ABaseBattlePawn::OnTurnEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("%s Turn End"), *GetName());
	// 입력 막고 FSM 종료
	
	// Casting을 통해 현재 유닛이 player 또는 enemy라면 그쪽 함수 실행
	if (auto* phaseManager = Cast<AUPhaseManager>(GetWorld()->GetGameState()))
	{
		// 턴 종료 상태라면 return
		if (phaseManager->turnManager->curTurnState == ETurnState::TurnEnd) return;
		
		if (ABattlePlayer* player = Cast<ABattlePlayer>(this))
		{
			// PlayerPhaseEnd
			phaseManager->EndPlayerPhase();
		}
		else if (ABaseEnemy* enemy = Cast<ABaseEnemy>(this))
		{
			// EnemyPhaseEnd
			phaseManager->EndEnemyPhase();
		}
	}
}

void ABaseBattlePawn::MouseClick(const FInputActionValue& value)
{
	if (!bIsMoveMode) return;

	FHitResult hitInfo;
	FVector start, end, dir;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	// 마우스 클릭 위치 얻기
	GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(start, dir);
	end = start + dir * 10000;

	if (GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Visibility, params))
	{
		AGridTile* clickedTile = Cast<AGridTile>(hitInfo.GetActor());
		if (clickedTile)
		{
			goalTile = clickedTile;
			goalTile->SetColor(FLinearColor::Red);

			// 시작 타일 다시 설정
			FHitResult startHit;
			FVector pawnStart = GetActorLocation();
			FVector pawnEnd = pawnStart + FVector::DownVector * 1000;

			if (GetWorld()->LineTraceSingleByChannel(startHit, pawnStart, pawnEnd, ECC_Visibility, params))
			{
				startTile = Cast<AGridTile>(startHit.GetActor());
				openArray.Add(startTile);
				// 길찾기 시작
				PathFind();
			}

			bIsMoveMode = false; // 클릭했으니까 모드 종료
		}
	}
}

void ABaseBattlePawn::PathFind()
{
	while (openArray.Num() > 0)
	{
		currentTile = openArray[0];
		openArray.RemoveAt(0);

		if (!IsValid(currentTile))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s currentTile PathFind Error"), *GetName());
			return;
		}
		if (!IsValid(goalTile))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s goalTile PathFind Error"), *GetName());
			return;
		}
		// 목표 도달했으면 종료
		if (currentTile == goalTile)
		{
			BuildPath();
			return;
		}

		if (IsValid(currentTile))
		{
			AddOpenArray(FVector::ForwardVector);
			AddOpenArray(FVector::BackwardVector);
			AddOpenArray(FVector::LeftVector);
			AddOpenArray(FVector::RightVector);

			closedArray.Add(currentTile);
			currentTile->SetColor(FLinearColor::Blue);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("길을 찾지 못했습니다"));
}

void ABaseBattlePawn::BuildPath()
{
	AGridTile* temp = goalTile;
	while (temp && temp->parentTile)
	{
		temp->SetColor(FLinearColor::Yellow);
		temp = temp->parentTile;
	}
}

void ABaseBattlePawn::AddOpenArray(FVector dir)
{
	FVector start = currentTile->GetActorLocation();
	FVector end = start + dir * 110;
	
	FHitResult hitInfo;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	params.AddIgnoredActor(currentTile);

	DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 5.0f);
	
	if (bool isHit = GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Visibility, params))
	{
		AGridTile* tile = Cast<AGridTile>(hitInfo.GetActor());
		if (!IsValid(tile))
		{
			UE_LOG(LogTemp, Warning, TEXT("타일이 유효하지 않습니다. %s"), *dir.ToString());
		}
		// openArray, closeArray에 해당 tile이 없을 때만
		if (openArray.Contains(tile) == false && closedArray.Contains(tile) == false)
		{
			// 해당 tile에 Cost 구하기
			tile->SetCost(currentTile, goalTile);
			// openArray 값에 tile 추가 (cost 값이 작은 tile 앞쪽에)
			int32 i = 0;
			for (i = 0; i < openArray.Num(); ++i)
			{
				if (openArray[i]->tCostValue >= tile->tCostValue)
				{
					break;
				}
			}
			openArray.Insert(tile, i);
		}
	}
}

void ABaseBattlePawn::UnitMove()
{
	// 스킬을 누르면(Complete) 타일 색이 변하며 원하는 타일 위치를 눌렀을 때 그 위치로 길찾기
	if (openArray[0] == goalTile || openArray.Num() == 0)
	{
		// 찾은 길 표시
		AGridTile* temp = goalTile;
		while (temp->parentTile != nullptr)
		{
			temp->SetColor(FLinearColor::Yellow);
			temp = temp->parentTile;
		}
	}
	// 그렇지 않으면
	else
	{
		// 계속 길찾기 진행
		PathFind();
	}
}
