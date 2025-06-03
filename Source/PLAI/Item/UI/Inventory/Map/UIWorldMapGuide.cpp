// Fill out your copyright notice in the Description page of Project Settings.


#include "UIWorldMapGuide.h"

void UUIWorldMapGuide::NativeDestruct()
{
	Super::NativeDestruct();

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}
