// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/UI/ReasonUI.h"

#include "Components/TextBlock.h"


void UReasonUI::SetText(FString text)
{
	FString s = FString::Printf(TEXT("엔트 : %s"), *text);
	txt_Reason->SetText(FText::FromString(s));
}
