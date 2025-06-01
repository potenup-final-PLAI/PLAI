// Fill out your copyright notice in the Description page of Project Settings.


#include "EndingCredit.h"

#include "Animation/WidgetAnimation.h"


void UEndingCredit::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (PaidIn)
	{
		FWidgetAnimationDynamicEvent EndDelegate;
		EndDelegate.BindDynamic(this, &UEndingCredit::PlayEndingCredit);
		BindToAnimationFinished(PaidIn, EndDelegate);
	}
}

void UEndingCredit::Mulitcast_PaidIn_Implementation()
{
	if (PaidIn)
	{
		PlayAnimation(PaidIn);
	}
}

void UEndingCredit::PlayEndingCredit()
{
	if (EndingCredit)
	{
		PlayAnimation(EndingCredit);
	}
}
