// Fill out your copyright notice in the Description page of Project Settings.


#include "UiWorldMap.h"

#include "Components/Image.h"

void UUiWorldMap::NativeConstruct()
{
	Super::NativeConstruct();

	MaterialMapDynamic = UMaterialInstanceDynamic::Create(MaterialMapInterface,this);

	MiniMap->SetBrushFromMaterial(MaterialMapDynamic);

	MaterialMapDynamic->SetScalarParameterValue("ZoomFactor", 1.0f);
	MaterialMapDynamic->SetVectorParameterValue("CenterOffset", FVector4(0.5f,0.5f,0,0));
	
}
