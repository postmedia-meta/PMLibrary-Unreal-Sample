// Fill out your copyright notice in the Description page of Project Settings.


#include "MetaNGXWidget.h"

#include "MetaNVIDIASettingsComponent.h"
#include "Components/Button.h"

void UMetaNGXWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MNESaveGame = UMNESaveGame::Get();

	const auto PlayerController = GetOwningPlayer();
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("This component can only be used as a component of a player controller."));
		return;
	}

	MetaNVIDIASettingsComponent = PlayerController->GetComponentByClass<UMetaNVIDIASettingsComponent>();
	if (MetaNVIDIASettingsComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("MetaNVIDIASettingsComponent is nullptr"));
	}

	ApplyButton->OnClicked.AddDynamic(this, &UMetaNGXWidget::ApplySettings);
}

void UMetaNGXWidget::NativeDestruct()
{
	Super::NativeDestruct();

	ApplyButton->OnClicked.RemoveDynamic(this, &UMetaNGXWidget::ApplySettings);

}

void UMetaNGXWidget::ApplySettings()
{
	MNESaveGame->NVIDIASettings = MetaNVIDIASettingsComponent->NVIDIASettings;
	MNESaveGame->SaveGame();
}

void UMetaNGXWidget::ShowWidget()
{
	SetVisibility(ESlateVisibility::Visible);	
}

void UMetaNGXWidget::HideWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);	
}
