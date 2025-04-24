// Fill out your copyright notice in the Description page of Project Settings.


#include "UiPost.h"

#include "Components/EditableTextBox.h"

void UUiPost::NativeConstruct()
{
	Super::NativeConstruct();
	
	TextPost->OnTextCommitted.AddDynamic(this, &UUiPost::UiPostEnter);
}

void UUiPost::UiPostEnter(const FText& Text, ETextCommit::Type CommitMethod)
{
	OnNetPost.ExecuteIfBound(Text.ToString());
}
