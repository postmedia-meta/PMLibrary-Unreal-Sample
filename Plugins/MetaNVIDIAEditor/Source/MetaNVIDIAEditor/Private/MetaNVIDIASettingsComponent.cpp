// Fill out your copyright notice in the Description page of Project Settings.


#include "MetaNVIDIASettingsComponent.h"

#include "MetaNGXWidget.h"

UMetaNVIDIASettingsComponent::UMetaNVIDIASettingsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UMetaNVIDIASettingsComponent::BeginPlay()
{
	Super::BeginPlay();
	
	MNESaveGame = UMNESaveGame::Get();

	NVIDIASettings = MNESaveGame->NVIDIASettings;

	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("This component can only be used as a component of a player controller."));
		return;
	}

	if (MetaNGXWidgetClass == nullptr)
	{
		MetaNGXWidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/Script/UMGEditor.WidgetBlueprint'/MetaNVIDIAEditor/Blueprints/UI/BPW_NGX.BPW_NGX_C'"));
	}
	if (MetaNGXWidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("MetaNGXWidgetClass is nullptr"));
		return;
	}

	MetaNGXWidget = CreateWidget<UMetaNGXWidget>(PlayerController, MetaNGXWidgetClass);
	if (MetaNGXWidget)
	{
		MetaNGXWidget->AddToViewport(ZOrder);
		MetaNGXWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMetaNVIDIASettingsComponent::ShowNGXUI()
{
	if (MetaNGXWidget)
	{
		MetaNGXWidget->ShowWidget();
	}
}

void UMetaNVIDIASettingsComponent::HideNGXUI()
{
	if (MetaNGXWidget)
	{
		MetaNGXWidget->HideWidget();
	}
}

bool UMetaNVIDIASettingsComponent::IsShowWidget()
{
	if (MetaNGXWidget)
	{
		return MetaNGXWidget->GetVisibility() != ESlateVisibility::Collapsed;
	}

	return false;
}