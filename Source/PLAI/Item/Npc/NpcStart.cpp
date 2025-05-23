// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcStart.h"


// Sets default values
ANpcStart::ANpcStart()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANpcStart::BeginPlay()
{
	Super::BeginPlay();
	
	UiNpcStart = CreateWidget<UUiNpcStart>(GetWorld(),UiNpcStartFactory);
	UiNpcStart->AddToViewport();
    UiNpcStart->SetVisibility(ESlateVisibility::Hidden);
	NpcUiMaster = Cast<UUserWidget>(UiNpcStart);
	
	NpcNameString = TEXT("알수 없는 검");
}

// Called every frame
void ANpcStart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ANpcStart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ANpcStart::WarriorStarter()
{
	TArray<FName> RawNames = ItemTable->GetRowNames();
	for (FName RawName : RawNames)
	{
		FItemStructTable* A = ItemTable->FindRow<FItemStructTable>(RawName,TEXT("NpcStart"));
		if (A->ItemTop == 1 && TArray<int32>({0,1,2}).Contains(A->ItemIndex) && A->ItemIndexType == 0 &&
			A->ItemIndexDetail == 0)
		{
			ItemStructTables.ItemStructTables.Add(*A);
		}
	}
	//이 딜리게이트는 InvenComp NpcItem함수에 바인딩함
	OnNpcStart.ExecuteIfBound(ItemStructTables);
}


void ANpcStart::HunterStarter()
{
	// 헌터
	TArray<FName> RawNames = ItemTable->GetRowNames();
	for (FName RawName : RawNames)
	{
		FItemStructTable* A = ItemTable->FindRow<FItemStructTable>(RawName,TEXT("NpcStart"));
		if (A->ItemTop == 1 && TArray<int32>({0,1,2}).Contains(A->ItemIndex) && A->ItemIndexType == 1
			&& A->ItemIndexDetail == 0)
		{
			ItemStructTables.ItemStructTables.Add(*A);
		}
	}
	//이 딜리게이트는 InvenComp NpcItem함수에 바인딩함
	OnNpcStart.ExecuteIfBound(ItemStructTables);
}


