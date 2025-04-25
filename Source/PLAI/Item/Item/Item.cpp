// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"


// Sets default values
AItem::AItem()
{
	bReplicates = true;
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>("BoxComp");
	BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(BoxComp);
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(BoxComp);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	ItemParent = ItemFactory->GetDefaultObject<AItem>();
	// BoxComp->SetWorldScale3D(FVector(1.5));
	BoxComp->OnComponentBeginOverlap.AddDynamic(this,&AItem::OnMyBeginOverlapped);
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,this,&AItem::SetMesh,0.5f,false);
}

void AItem::OnMyBeginOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		// ATestPlayer* TestPlayer = Cast<ATestPlayer>(OtherActor);
		// if (TestPlayer && TestPlayer->HasAuthority())
		// {
		// 	if (TestPlayer->InvenComp->MenuInven == nullptr)
		// 	{ UE_LOG(LogTemp,Display,TEXT("Item 플레이어 랩박스, 슬롯이 없네?"))}
		// 	TestPlayer->InvenComp->Server_GetItem(ItemStruct);
		// 	// Destroy();
		// }
	}
}

void AItem::SetMesh()
{
	if (ItemStruct.ItemTop == -1) { UE_LOG(LogTemp,Warning,TEXT("아이템 탑 -1 초기화전")) return; }
	StaticMesh->SetStaticMesh(ItemParent->ItemStructTop.ItemMeshTops[ItemStruct.ItemTop].
	ItemMeshIndexes[ItemStruct.ItemIndex].ItemMeshTypes[ItemStruct.ItemIndexType].StaticMeshes[ItemStruct.ItemIndexDetail]);

}

void AItem::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItem,ItemStruct)
}

// Called every frame


