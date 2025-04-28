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

	if (bIsMoving)
	{
		UnitMove();
	}
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
		AGridTile* clickTile = Cast<AGridTile>(hitInfo.GetActor());
		if (clickTile)
		{
			goalTile = clickTile;
			goalTile->SetColor(FLinearColor::Red);

			// 시작 타일 다시 설정
			FHitResult startHit;
			FVector pawnStart = GetActorLocation();
			FVector pawnEnd = pawnStart + FVector::DownVector * 1000;

			if (GetWorld()->LineTraceSingleByChannel(startHit, pawnStart, pawnEnd, ECC_Visibility, params))
			{
				clickTile = Cast<AGridTile>(startHit.GetActor());
				if (IsValid(clickTile))
				{
					openArray.Add(clickTile);
					// 길찾기 시작
					PathFind();
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("StartTile is Invalid Cannot start pathFinding"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Fail to LineTrace downwards to find clickTile"));
			}

			bIsMoveMode = false; // 클릭했으니까 모드 종료
		}
		
	}
}

void ABaseBattlePawn::PathFind()
{
	if (openArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenArray is empty"));
		return;
	}

	int32 safetyCounter = 0;
	const int32 maxSafetyCount = 1000;
	
	while (openArray.Num() > 0)
	{
		if (++safetyCounter > maxSafetyCount)
		{
			UE_LOG(LogTemp, Warning, TEXT("Path Safe Break"));
			break;
		}
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
	// 찾은 길 표시
	AGridTile* temp = goalTile;
	while (temp && temp->parentTile)
	{
		pathArray.Insert(temp, 0); // 역방향으로 삽입
		temp->SetColor(FLinearColor::Yellow);
		temp = temp->parentTile;
	}

	// 경로 저장 완료했으면 이동 시작
	if (pathArray.Num() > moveRange)
	{
		UE_LOG(LogTemp, Warning, TEXT("Path too long! Max Move Range = %d, Path Length = %d"), moveRange, pathArray.Num());
        
		// 경로 초기화하고 이동 금지
		pathArray.Empty();
		bIsMoving = false;

		// 목표 타일 색 초기화
		if (goalTile)
		{
			goalTile->SetColor(FLinearColor::White);
		}

		return;
	}
	
	if (pathArray.Num() > 0)
	{
		bIsMoving = true;
		currentPathIndex = 0;
	}
}

void ABaseBattlePawn::AddOpenArray(FVector dir)
{
	FVector start = currentTile->GetActorLocation();
	FVector end = start + dir * 200;
	
	FHitResult hitInfo;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	params.AddIgnoredActor(currentTile);

	DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 5.0f);
	
	if (bool isHit = GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Visibility, params))
	{
		AGridTile* tile = Cast<AGridTile>(hitInfo.GetActor());
		// openArray, closeArray에 해당 tile이 없을 때만
		if (IsValid(tile) && !openArray.Contains(tile) && !closedArray.Contains(tile))
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
	// 유닛이 찾은 길을 따라 움직이기
	if (pathArray.IsValidIndex(currentPathIndex))
	{
		FVector targetLoc = pathArray[currentPathIndex]->GetActorLocation();
		FVector currentLoc = GetActorLocation();

		FVector newLoc = FMath::VInterpConstantTo(currentLoc, targetLoc, GetWorld()->GetDeltaSeconds(), moveSpeed);

		SetActorLocation(newLoc);
		
		// 목표 지점에 거의 도착하면 다음 목표로 이동
		if (FVector::DistSquared(newLoc, targetLoc) < FMath::Square(5.0f))
		{
			SetActorLocation(targetLoc);
			++currentPathIndex;

			// 인덱스가 array보다 크거나 같다면
			if (currentPathIndex >= pathArray.Num())
			{
				// 이동 종료
				bIsMoving = false;
				// AI 액션 처리를 위한 함수
				OnMoveEnd();
			}
		}
	}
	else
	{
		bIsMoving = false;
	}
}

void ABaseBattlePawn::OnMoveEnd()
{
	// AI에 턴을 종료하거나 다음 액션 처리 등을 여기서 처리
	UE_LOG(LogTemp, Warning, TEXT("Move Complete"));
}

void ABaseBattlePawn::InitValues()
{
	openArray.Empty();
	closedArray.Empty();
	pathArray.Empty();
	currentPathIndex = 0;
	bIsMoving = false;
	startTile = nullptr;
	goalTile = nullptr;
}
