// Fill out your copyright notice in the Description page of Project Settings.


#include "MonDamage.h"

void UMonDamage::NativeConstruct()
{
	Super::NativeConstruct();
	PlayAnimation(DamageUiAnimation,0,2);
}
