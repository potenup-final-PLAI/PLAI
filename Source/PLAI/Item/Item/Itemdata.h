#pragma once

#include "CoreMinimal.h"
#include "Itemdata.generated.h"


// ItemShield = GetWorld()->SpawnActor<AItemMaster>(ItemMasterFactory,TestPlayer->GetActorLocation() + FVector(0,0,500),FRotator(0,0,0));
// ItemShield->AttachToComponent(TestPlayer->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
// ItemShield->ItemStruct.ItemTop = 1; ItemShield->ItemStruct.ItemIndexDetail = 1;
// ItemShield->SetActorRelativeLocation(FVector(0,75,0));

// CompWeapon = NewObject<UStaticMeshComponent>(TestPlayer,TEXT("CompWeapon"));
// CompWeapon->RegisterComponent();
// CompWeapon->AttachToComponent(TestPlayer->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
// CompWeapon->SetRelativeLocation(FVector(0,75,0));
// CompWeapon->SetStaticMesh(Item->ItemParent->ItemStructTop.ItemMeshTops[2].ItemMeshIndexes[0].ItemMeshTypes[0].StaticMeshes[0]);