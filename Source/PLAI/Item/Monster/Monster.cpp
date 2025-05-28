// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ProgressBar.h"
#include "Components/SphereComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "MonUi/MonDamage.h"

#include "MonUi/MonUi.h"
#include "PLAI/Item/Item/Item.h"
#include "PLAI/Item/Item/ItemMaster.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"


class AItemMaster;
// Sets default values
AMonster::AMonster()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	// SphereComponent->SetupAttachment(RootComponent);
	
	MonUiComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("MonUiComponent"));
	MonUiComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMonster::BeginPlay()
{
	Super::BeginPlay();

	SetActorScale3D(FVector(1.5f, 1.5f, 1.5f));
	TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter());

	MonsterParent = MonsterFactory->GetDefaultObject<AMonster>();
	MonUi = CreateWidget<UMonUi>(GetWorld(),MonsterParent->MonUiFactory);
	
	MonUiComp->SetWidget(MonUi);
	MonUiComp->SetDrawSize(FVector2D(160.f, 40.f));
	MonUiComp->SetVisibility(true);
	MonUiComp->SetRelativeLocation(FVector(0,0,300));
	MonUiComp->SetWidgetSpace(EWidgetSpace::Screen); // or World

	SetMonsterUi();
	SetHpBar();
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
	{ MonsterStruct.gold = FMath::FRandRange(0.85,1.15) * MonsterStruct.gold; },0.1,false);
}

// Called every frame
void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MonUiComp->SetWorldRotation((-TestPlayer->TopDownCameraComponent->GetForwardVector()).Rotation());
}

// Called to bind functionality to input
void AMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void AMonster::SetMonsterUi()
{
	MonUi->Name->SetText(FText::FromString(MonsterStruct.Name));
	MonUi->CurrentHp->SetText(FText::AsNumber(MonsterStruct.CurrentHp));
	MonUi->MaxHp->SetText(FText::AsNumber(MonsterStruct.MaxHp));
}

void AMonster::SetHpBar()
{
	MonUi->HpBar->SetPercent(static_cast<float>(MonsterStruct.CurrentHp) / MonsterStruct.MaxHp);
	MonUi->CurrentHp->SetText(FText::AsNumber(MonsterStruct.CurrentHp));
}

void AMonster::Dead()
{
	if (MonsterStruct.MonDropTableFactory.Num() == 0) return;
	int32 Randindex = FMath::RandRange(0,MonsterStruct.MonDropTableFactory.Num()-1);
	FItemStructTable* ItemStructTable = MonsterStruct.MonDropTableFactory[Randindex].
	ItemRowHandle.DataTable->FindRow<FItemStructTable>(MonsterStruct.MonDropTableFactory[Randindex].
	ItemRowHandle.RowName,TEXT("Monster"));
	
	if (AItem* ItemMaster = GetWorld()->SpawnActor<AItem>(MonsterParent->ItemMasterFactory))
	{
		ItemMaster->ItemStructTable = *ItemStructTable;
		ItemMaster->StaticMesh->SetStaticMesh(ItemStructTable->StaticMesh);
		if (ItemStructTable->Material)
		{ ItemMaster->StaticMesh->SetMaterial(0,ItemStructTable->Material);}
		ItemMaster->SetActorLocation(GetActorLocation()+FVector(0,0,75));
		if (ItemStructTable->ItemTop !=1)
		{ ItemMaster->SetActorScale3D(FVector(4,4,4)); }
		else
		{ ItemMaster->SetActorScale3D(FVector(1.3,1.3,1.3));}
		ItemMaster->BoxComp->SetSimulatePhysics(true);
		
		if (ItemStructTable->ItemIndexDetail == 2)
		{
			ItemMaster->NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(ItemMaster->ItemParent->NiagaraSys,ItemMaster->BoxComp,
			NAME_None,ItemMaster->GetActorLocation(),FRotator::ZeroRotator, FVector(2.0f),EAttachLocation::KeepWorldPosition,
			true,ENCPoolMethod::None,true);
			// ItemMaster->NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(ItemMaster->ItemParent->NiagaraSys,
			// 	ItemMaster->StaticMesh,FRotator(0,0,0), FVector(1.f), true,true,ENCPoolMethod::AutoRelease);
			// ItemMaster->NiagaraComp->SetActive(true);
		}
	}
	Destroy();
}

void AMonster::DamageUi(float Damage)
{
	MonDamage = CreateWidget<UMonDamage>(GetWorld(),MonsterParent->MonDamageFactory);
	MonDamage->Damage->SetText(FText::AsNumber(Damage));
	
	UWidgetComponent* MonDamageComp = NewObject<UWidgetComponent>(this);
	MonDamageComp->SetupAttachment(GetRootComponent());
	
	MonDamageComp->RegisterComponent();
	MonDamageComp->SetWidget(MonDamage);
	float DamageLoc = FMath::FRandRange(-50.0f,50.0f);
	MonDamageComp->SetRelativeLocation(FVector(DamageLoc,DamageLoc,DamageLoc));
	MonDamageComp->SetDrawSize(FVector2D(150.f,30.f));
	MonDamageComp->SetWidgetSpace(EWidgetSpace::Screen);
	
	DamageUiDestroy(MonDamageComp);
	// FTimerDelegate TimerHandle;
	// TimerHandle.BindUFunction(this,FName("DestroyDamageComp"), MonDamageComp);
	// GetWorld()->GetTimerManager().SetTimer(DamageUiTimerHandle,TimerHandle,
	// 	1.0f,false);
}

void AMonster::DamageUiDestroy(UWidgetComponent* DamageComp)
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this,DamageComp]()
	{
		DamageComp->DestroyComponent();
	},1.0f,false);
}

// FItemStructTable* ItemStructTable = MonsterParent->ItemTable->FindRow<FItemStructTable>(MonsterStruct.
// 		MonDropTableFactory[0].ItemRowHandle.RowName,TEXT("Monster"));
// 	
// if (AItemMaster* ItemMaster = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory))
// {
// 	ItemMaster->ItemStructTable = *ItemStructTable;
// 	ItemMaster->StaticMesh->SetStaticMesh(ItemStructTable->StaticMesh);
// }
// FTimerHandle TimerHandle;
// GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
// {
// 	Destroy();
// },1.0f,false);


// if (!MonsterStruct.MonDropTableFactory.IsValidIndex(0))
// {
// 	UE_LOG(LogTemp, Error, TEXT("DropItem: MonDropTableFactory[0]이 없습니다."));
// 	return;
// }
// else
// {
// 	UE_LOG(LogTemp, Error, TEXT("DropItem:  MonDropTableFactory Num값 %d"),
// 		MonsterStruct.MonDropTableFactory.Num());
// }
// 3) RowName이 비어있지 않은지
// if (!MonsterParent->MonsterStruct.MonDropTableFactory.IsValidIndex(0))
// {
// 	UE_LOG(LogTemp, Error, TEXT("DropItem: MonDropTableFactory 배열이 비어있음!"));
// 	return;
// }
// FName RowName = MonsterParent->MonsterStruct.MonDropTableFactory[0].ItemRowHandle.RowName;
// if (RowName.IsNone())
// {
// 	UE_LOG(LogTemp, Error, TEXT("DropItem: RowHandle.RowName이 NAME_None입니다."));
// 	return;
// }