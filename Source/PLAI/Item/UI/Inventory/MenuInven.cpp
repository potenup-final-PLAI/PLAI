// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuInven.h"

#include "PLAI/Item/UI/Slot/SlotCre.h"

void UMenuInven::NativeConstruct()
{
	Super::NativeConstruct();
	WBP_SlotCre->MenuInven = this;
}
