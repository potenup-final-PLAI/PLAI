// Fill out your copyright notice in the Description page of Project Settings.


#include "CreFsm.h"

#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Engine/OverlapResult.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/Login/LoginComp.h"
#include "PLAI/Item/Monster/Monster.h"
#include "PLAI/Item/Monster/MonUi/MonDamage.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemInven.h"
#include "PLAI/Item/UI/Inventory/UiCre/UiCre.h"


// Sets default values for this component's properties
UCreFsm::UCreFsm()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCreFsm::BeginPlay()
{
	Super::BeginPlay();
	// TestPlayer = Cast<ATestPlayer>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	Creature = Cast<ACreature>(GetOwner());
	// ...
}


// Called every frame
void UCreFsm::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCreFsm::AttackMonster(AMonster* Monster)
{
	bool Critical = false;
	int Damage = 0;
	if (CreStruct.Crit > FMath::FRandRange(0.0f,100.0f))
	{ Damage = CreStruct.Atk + CreStruct.Atk*CreStruct.CritDmg/100; Critical = true; }
	else { Damage = CreStruct.Atk; }
    Damage = FMath::FRandRange(Damage * 0.9,Damage * 1.1);
	
	Monster->MonsterStruct.CurrentHp -= Damage;
	Monster->SetHpBar();

	// 몬스터 데미지주기 과연될까?
	Creature->MonDamage = CreateWidget<UMonDamage>(GetWorld(),Creature->CreatureParent->MonDamageFactory);
	Creature->MonDamage->Damage->SetText(FText::AsNumber(Damage));
	UWidgetComponent* MonDamageComp = NewObject<UWidgetComponent>(this);
	MonDamageComp->SetupAttachment(Monster->GetRootComponent());
	MonDamageComp->RegisterComponent();
	MonDamageComp->SetWidget(Creature->MonDamage);
	float DamageLoc = FMath::FRandRange(-50.0f,50.0f);
	MonDamageComp->SetRelativeLocation(FVector(DamageLoc,DamageLoc,DamageLoc));
	MonDamageComp->SetDrawSize(FVector2D(150.f,30.f));
	MonDamageComp->SetWidgetSpace(EWidgetSpace::Screen);
	DamageUiDestroy(MonDamageComp);
	
	// Monster->DamageUi(Damage);
	if (Critical)
	{
		Creature->MonDamage->SetRenderScale(FVector2D(1.3,1.3));
		Creature->MonDamage->Damage->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
		Creature->MonDamage->Damage->SetShadowColorAndOpacity(FLinearColor::Black);
	}
	
	if (Monster->MonsterStruct.CurrentHp <= 0)
	{
		CreStruct.CurrentExp += Monster->MonsterStruct.Exp;
		Monster->Dead();
		GetMonGold(Monster);
		SetCreStat();
	}
}

void UCreFsm::DamageUiDestroy(UWidgetComponent* DamageComp)
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this,DamageComp]()
	{
		DamageComp->DestroyComponent();
	},1.0f,false);
}

void UCreFsm::SetCreStat()
{
	if (CreStruct.CurrentExp > CreStruct.MaxExp)
	{
		CreStruct.MaxExp = CreStruct.MaxExp * 1.2;
		CreStruct.CurrentExp = 0;
		CreStruct.Level++;
	}
	TestPlayer->InvenComp->MenuInven->Wbp_UiCre->SetUiCre(&CreStruct);
}

void UCreFsm::GetMonGold(AMonster* Monster)
{
	TestPlayer->LoginComp->UserFullInfo.inventory_info.gold += Monster->MonsterStruct.gold;
	int32 GetGold = TestPlayer->LoginComp->UserFullInfo.inventory_info.gold;
	TestPlayer->InvenComp->MenuInven->WBP_ItemInven->WbpItemGold->Gold->SetText(FText::AsNumber(GetGold));
	TestPlayer->InvenComp->MenuInven->Wbp_ItemGold->Gold->SetText(FText::AsNumber(GetGold));
}

float UCreFsm::PlayerDistance()
{
	return FVector::Distance(TestPlayer->GetActorLocation(),Creature->GetActorLocation());
}

FMonsters UCreFsm::GetMonsterBySphere(AActor* Actor,float Radios)
{
	FMonsters Monsters;
	TArray<FOverlapResult>Hits;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Creature);
	Params.AddIgnoredActor(TestPlayer);

	bool bHit = GetWorld()->OverlapMultiByChannel(Hits,Actor->GetActorLocation(),FQuat::Identity,
		ECC_Visibility,FCollisionShape::MakeSphere(Radios),Params);

	if(bHit)
	{
		for (FOverlapResult Hit : Hits)
		{
			if (AMonster* Monster = Cast<AMonster>(Hit.GetActor()))
			{
				if (!Monsters.Monsters.Contains(Monster))
				{
					Monsters.Monsters.Add(Monster);
				}
			}
		}
	}
	return Monsters;
}

FVector UCreFsm::LineTraceZ(AActor* Actor,FVector Vector)
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Actor);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit,Vector,Vector+FVector(0,0,-10000),
	ECC_Visibility, Params);
	if(bHit)
	{
		return Hit.Location;
	}
	return Vector;
}

float UCreFsm::CreatureDamage()
{
    if (CreStruct.Crit > FMath::FRandRange(0.0f,100.0f))
	{
		return CreStruct.Atk + CreStruct.Atk*CreStruct.CritDmg/100;
	}
	else
	{
		return CreStruct.Atk;
	}
}


