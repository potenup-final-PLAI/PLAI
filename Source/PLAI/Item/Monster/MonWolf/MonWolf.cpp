// Fill out your copyright notice in the Description page of Project Settings.


#include "MonWolf.h"

#include "KismetTraceUtils.h"
#include "MonFsmWolf.h"


// Sets default values
AMonWolf::AMonWolf()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MonFsm = CreateDefaultSubobject<UMonFsmWolf>("MonFsm");

	SocketNames = {
		TEXT("FootFrontLeft"),
		TEXT("FootFrontRight"),
		TEXT("FootBackRight"),
		TEXT("FootBackLeft"),
	};
}

// Called when the game starts or when spawned
void AMonWolf::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMonWolf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetGravity();
}

// Called to bind functionality to input
void AMonWolf::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMonWolf::SetGravity()
{
	FVector OffSet = FVector::ZeroVector;
	
	int32 Count = 0;
	for (const FName SocketName : SocketNames)
	{
		FVector Start = GetMesh()->GetSocketLocation(SocketName);
		FVector End = Start + FVector(0,0,-5000);

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		
		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Params);
		if (bHit)
		{
			// DrawDebugLine(GetWorld(), Start,Hit.ImpactPoint, FColor::Blue, false, 0.02f);
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 15, 10, FColor::Green, false,0.02f);

			FVector Distance = Hit.ImpactPoint - GetMesh()->GetSocketLocation(SocketName);
			if (Distance.Length() > 25.0f)
			{
				OffSet += Distance.GetSafeNormal() * 5;
				Count++;
			}
		}
	}
	AddActorWorldOffset(OffSet/Count, true);
}

// void AMonWolf::SetGravity()
// {
// 	// 1) Mesh 유효성 확인
// 	USkeletalMeshComponent* SkelMesh = GetMesh();
// 	if (!IsValid(SkelMesh))
// 	{ UE_LOG(LogTemp, Error, TEXT("SetGravity: Mesh가 유효하지 않습니다."));
// 		return; }
// 	// constexpr float DesiredHeight = 5.f;
// 	int32 ContactCount = 0;
//
// 	// 2) 각 소켓에서 라인트레이스
// 	for (const FName& SocketName : SocketNames)
// 	{
// 		const FVector FootLoc = SkelMesh->GetSocketLocation(SocketName);
// 		const FVector End = FootLoc - FVector(0.f, 0.f, 2500.f);
//
// 		FHitResult Hit;
// 		FCollisionQueryParams Params;
// 		Params.AddIgnoredActor(this);
//
// 		bool bHit = GetWorld()->LineTraceSingleByChannel(
// 			Hit, FootLoc, End, ECC_Visibility, Params);
// 		
// 		if (bHit)
// 		{
// 			DrawDebugSphere(GetWorld(), GetMesh()->GetSocketLocation(SocketName), 15, 10, FColor::Red, false);
// 			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 15, 10, FColor::Green, false);
// 			DrawDebugLine(GetWorld(), FootLoc, Hit.ImpactPoint, FColor::Green , false);
// 			float Dist = Hit.Distance;
// 			if (Dist > 5.f)
// 			{
// 				float Diff = Dist - 5.0f;
// 				OffSet = -Hit.ImpactNormal * Diff;
// 				// TotalOffset += Offset;
// 				ContactCount++;
// 			}
// 		}
// 	}
//
// 	// 3) 평균 오프셋을 계산하여 한 번에 적용
// 	if (ContactCount > 0)
// 	{
// 		FVector AvgOffset = OffSet / ContactCount;
// 		// 실물 Physics 없이 위치 보정
// 		AddActorWorldOffset(AvgOffset, true);
// 	}
// }

