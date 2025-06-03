// Fill out your copyright notice in the Description page of Project Settings.


#include "UiQuest.h"

#include <string>

#include "Blueprint/WidgetTree.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

void UUiQuest::NextQuest(int32 QuestNum, FString QuestTitle, FString QuestContent)
{
	FString String = FString::Printf(TEXT("%d Quest : %s"), QuestNum,*QuestTitle);
	QuestName->SetText(FText::FromString(String));
	QuestContentBlock->SetText(FText::FromString(QuestContent));

	if (!IsValid(this)) return;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		CurrentTime += GetWorld()->GetDeltaSeconds();
		QuestName->SetRenderOpacity( CurrentTime * 0.33 );
		QuestContentBlock->SetRenderOpacity( CurrentTime * 0.33 );
		if (CurrentTime > 5)
		{
			CurrentTime = 0;
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			QuestName->SetRenderOpacity(0);
			QuestContentBlock->SetRenderOpacity(0);
		}
	}),0.02f,true);

	for (int i = 0; i < QuestBox->GetChildrenCount(); i++)
	{
		if (UTextBlock* Text =  Cast<UTextBlock>(QuestBox->GetChildAt(i)))
		{
			if (Text->GetText().ToString() == QuestContent)
			{
				return;
			}
		}
	}

	for (int i = 0; i < QuestBox->GetChildrenCount(); i++)
	{
		if (UTextBlock* Text =  Cast<UTextBlock>(QuestBox->GetChildAt(i)))
		{
			Text->SetColorAndOpacity(FLinearColor::White);
			Text->SetRenderOpacity(0.5);
		}
	}
	
	UTextBlock* QuestText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	QuestText->SetText(FText::FromString(FString::Printf(TEXT("%d %s"),QuestNum,*QuestTitle)));
	QuestText->SetColorAndOpacity(FLinearColor::White);
	QuestText->SetFont(FSlateFontInfo(QuestText->GetFont().FontObject,24));
	QuestText->SetShadowColorAndOpacity(FLinearColor::Black);
	QuestBox->AddChild(QuestText);
	
	UTextBlock* QuestTextContent = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	QuestTextContent->SetText(FText::FromString(QuestContent));
	QuestTextContent->SetColorAndOpacity(FLinearColor::White);
	QuestTextContent->SetFont(FSlateFontInfo(QuestTextContent->GetFont().FontObject,18));
	QuestText->SetShadowColorAndOpacity(FLinearColor::Black);
	QuestTextContent->SetAutoWrapText(true);
	QuestBox->AddChild(QuestTextContent);
	QuestBox->ScrollToEnd();
	
	UTextBlock* QuestTextEmpty = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	QuestBox->AddChild(QuestTextEmpty);
}

void UUiQuest::NativeConstruct()
{
	Super::NativeConstruct();

	QuestName->SetRenderOpacity(0.0f);
	QuestContentBlock->SetRenderOpacity(0.0f);
}

void UUiQuest::NativeDestruct()
{
	Super::NativeDestruct();
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}
