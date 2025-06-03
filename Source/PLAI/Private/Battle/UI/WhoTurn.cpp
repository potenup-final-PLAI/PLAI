// Fill out your copyright notice in the Description page of Project Settings.


#include "WhoTurn.h"

void UWhoTurn::PlayWhoTurnAnimation()
{
	if (WhoTurnAnimation)
	{
		PlayAnimation(WhoTurnAnimation);
	}
}
