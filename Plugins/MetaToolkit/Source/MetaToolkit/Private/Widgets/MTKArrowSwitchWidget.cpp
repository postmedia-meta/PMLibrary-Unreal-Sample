// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MTKArrowSwitchWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UMTKArrowSwitchWidget::NativeOnInitialized()
{
	Super::NativeConstruct();

	if (Elements.IsEmpty())
	{
		TextBlock->SetText(FText::FromString(TEXT("None")));
		return;
	}
	
	if (StartIndex >= Elements.Num()) StartIndex = 0;
	CurrentIndex = StartIndex;
	
	PrevButton->OnClicked.AddDynamic(this, &UMTKArrowSwitchWidget::ClickPrevButton);
	NextButton->OnClicked.AddDynamic(this, &UMTKArrowSwitchWidget::ClickNextButton);
}

void UMTKArrowSwitchWidget::ClickPrevButton()
{
	if (--CurrentIndex < 0) CurrentIndex = Elements.Num() - 1;
	TextBlock->SetText(FText::FromString(Elements[CurrentIndex]));

	if (OnChangedIndex.IsBound()) OnChangedIndex.Broadcast(CurrentIndex);
}

void UMTKArrowSwitchWidget::ClickNextButton()
{
	if (++CurrentIndex >= Elements.Num()) CurrentIndex = 0;
	TextBlock->SetText(FText::FromString(Elements[CurrentIndex]));

	if (OnChangedIndex.IsBound()) OnChangedIndex.Broadcast(CurrentIndex);
}

void UMTKArrowSwitchWidget::SetCurrentIndex(const int32 Index)
{
	if (Index >= Elements.Num()) return;
	
	CurrentIndex = Index;
	TextBlock->SetText(FText::FromString(Elements[CurrentIndex]));

	if (OnChangedIndex.IsBound()) OnChangedIndex.Broadcast(CurrentIndex);
}