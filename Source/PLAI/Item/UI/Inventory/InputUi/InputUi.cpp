// Fill out your copyright notice in the Description page of Project Settings.


#include "InputUi.h"

void UInputUi::InputUiHidden()
{
	SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
	{
		SignDegree += 3.6f;
		float Rad = FMath::DegreesToRadians(SignDegree);
		// UE_LOG(LogTemp, Display, TEXT("InputUi::InputUiHidden() SignDegree [%f] sin(SignDegree) [%f]"),SignDegree, FMath::Sin(Rad));
		
		CurrentOpacity -= 0.02f;
		
		SetRenderOpacity(sin(Rad));
		
		SetRenderOpacity(FMath::Clamp(sin(Rad) ,0.0f, 1.0f));
		
		if (CurrentOpacity <= 0.f)
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			SetVisibility(ESlateVisibility::Hidden);
			CurrentOpacity = 1.0f;
			SignDegree = 0.0f;
		}
		
	},0.02f,true);
}
