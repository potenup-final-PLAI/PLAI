// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenComp.h"

#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/UI/Inventory/EquipInven/EquipInven.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemInven.h"


// Sets default values for this component's properties
UInvenComp::UInvenComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInvenComp::BeginPlay()
{
	Super::BeginPlay();
	MenuInven = CreateWidget<UMenuInven>(GetWorld(),MenuInvenFactory);
	MenuInven->AddToViewport();
	// MenuInven->SetVisibility(ESlateVisibility::Hidden);
	
	TestPlayer = Cast<ATestPlayer>(GetOwner());
	if (TestPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInvenComp::플레이어 BeginPlay()"));
	}
    else
    {
    	UE_LOG(LogTemp, Warning, TEXT("UInvenComp::플레이어없음 BeginPlay()"));
    }
	
	PC = Cast<APlayerController>(GetOwner()->GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInvenComp: 컨트롤러 있음 BeginPlay()"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInvenComp: 컨트롤러 없음 BeginPlay()"));
	}
	// ...
	
}


// Called every frame
void UInvenComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// 아이템창 출력
	if (PC && PC->IsLocalController() && PC->WasInputKeyJustPressed(EKeys::I))
	{ UE_LOG(LogTemp, Warning, TEXT("인벤컴프 I키 !"));
		EnumKey = EEnumKey::Item;
			ItemInvenTory(EnumKey, MenuInven->WBP_ItemInven);
	}
    // 장비창 출력
	if (PC && PC->IsLocalController() && PC->WasInputKeyJustReleased(EKeys::E))
	{ UE_LOG(LogTemp, Warning, TEXT("인벤컴프 E키 "));
		EnumKey = EEnumKey::Equip;
		ItemInvenTory(EnumKey, MenuInven->WBP_EquipInven); }
}

void UInvenComp::ItemInvenTory(EEnumKey Key, UUserWidget* Inven)
{
	if (Flipflop == false)
	{
		UE_LOG(LogTemp,Warning,TEXT("UInvenComp::ItemInvenTory() 켯다"));
		Inven->SetVisibility(ESlateVisibility::Visible);
		Flipflop = true;
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("UInvenComp::ItemInvenTory() 껏다"));
		Inven->SetVisibility(ESlateVisibility::Hidden);
		Flipflop = false;
	}
}

