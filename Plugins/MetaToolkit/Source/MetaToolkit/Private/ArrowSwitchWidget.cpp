// Fill out your copyright notice in the Description page of Project Settings.


#include "ArrowSwitchWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UArrowSwitchWidget::NativeOnInitialized()
{
	Super::NativeConstruct();

	if (Elements.IsEmpty())
	{
		TextBlock->SetText(FText::FromString(TEXT("None")));
		return;
	}
	
	if (StartIndex >= Elements.Num()) StartIndex = 0;
	CurrentIndex = StartIndex;
	
	PrevButton->OnClicked.AddDynamic(this, &UArrowSwitchWidget::ClickPrevButton);
	NextButton->OnClicked.AddDynamic(this, &UArrowSwitchWidget::ClickNextButton);
}

void UArrowSwitchWidget::ClickPrevButton()
{
	if (--CurrentIndex < 0) CurrentIndex = Elements.Num() - 1;
	TextBlock->SetText(FText::FromString(Elements[CurrentIndex]));

	if (OnChangedIndex.IsBound()) OnChangedIndex.Broadcast(CurrentIndex);
}

void UArrowSwitchWidget::ClickNextButton()
{
	if (++CurrentIndex >= Elements.Num()) CurrentIndex = 0;
	TextBlock->SetText(FText::FromString(Elements[CurrentIndex]));

	if (OnChangedIndex.IsBound()) OnChangedIndex.Broadcast(CurrentIndex);
}

void UArrowSwitchWidget::SetCurrentIndex(const int32 Index)
{
	if (Index >= Elements.Num()) return;
	
	CurrentIndex = Index;
	TextBlock->SetText(FText::FromString(Elements[CurrentIndex]));

	if (OnChangedIndex.IsBound()) OnChangedIndex.Broadcast(CurrentIndex);
}