// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "Components/BoxComponent.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"


// Sets default values
AItem::AItem()
{
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
	
	BoxComp->SetWorldScale3D(FVector(2.2));
	BoxComp->OnComponentBeginOverlap.AddDynamic(this,&AItem::OnMyBeginOverlapped);
	
	FTimerHandle TimerHandle;
	UE_LOG(LogTemp,Log,TEXT("AItem::SetMesh 아이템구조체 인덱스 %d"),ItemStruct.ItemIndex);
	// GetWorld()->GetTimerManager().SetTimer(TimerHandle,this,&AItem::SetMesh,0.2f,false);
}

void AItem::OnMyBeginOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		// ATestPlayer* TestPlayer = Cast<ATestPlayer>(OtherActor);
		// if (TestPlayer)
		// {
		// UTexture2D* Texture = ItemMeshStructIndexArray[ItemStruct.ItemIndex].
		// 	ItemMeshStructIndex[ItemStruct.ItemIndexType].Textures[ItemStruct.ItemIndexDetail];
		// 	
		// 	TestPlayer->InvenComp->GetItem(ItemStruct, Texture, this);
		// 	Destroy();
		// 	// UE_LOG(LogTemp,Log,TEXT("AItem::오버랩 발생 엑터는? %s"),*OtherActor->GetName());
		// }
	}
}

void AItem::SetMesh()
{
	// if (ItemStruct.ItemTop == -1){UE_LOG(LogTemp,Warning,TEXT("아이템 탑 -1 초기화전")) return};
	int32 rand = FMath::RandRange(0,ItemParent->ItemStructTop.ItemMeshTops[ItemStruct.ItemTop].
		ItemMeshIndexes[ItemStruct.ItemIndex].ItemMeshTypes[ItemStruct.ItemIndexType].StaticMeshes.Num());
	
	    ItemStruct.ItemIndexDetail = rand;
	
	StaticMesh->SetStaticMesh(ItemParent->ItemStructTop.ItemMeshTops[ItemStruct.ItemTop].
	ItemMeshIndexes[ItemStruct.ItemIndex].ItemMeshTypes[ItemStruct.ItemIndexType].StaticMeshes[rand]);
}
// int32 AItem::RandIndex()
// {
// 	int32 a = ItemStruct.ItemIndex;
// 	int32 b = ItemStruct.ItemIndex;
// 	// int32 c = ItemStruct.ItemIndexDetail;
// 	int32 Index = FMath::RandRange(0,ItemParent->ItemMeshStructIndexArray[a].ItemMeshStructIndex[b].
// 		StaticMeshes.Num()-1);
// 	return Index;
// }


// Called every frame


