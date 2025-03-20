// Fill out your copyright notice in the Description page of Project Settings.


#include "MetaGraphicsSettingsWidget.h"

#include "ArrowSwitchWidget.h"
#include "MetaToolkitSaveGame.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameUserSettings.h"

void UMetaGraphicsSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MetaToolkitSaveGame = UMetaToolkitSaveGame::Get();
	if (MetaToolkitSaveGame == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("MetaToolkitSaveGame is nullptr!!!"));
		return;
	}

	PlayerController = GetOwningPlayer();
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr!!!"));
		return;
	}

	GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("GameUserSettings is nullptr!!!"));
		return;
	}
	
	InitVariable();
	ApplySettings();
	
	ResolutionX->OnTextCommitted.AddDynamic(this, &UMetaGraphicsSettingsWidget::ChangeResolutionX);
	ResolutionY->OnTextCommitted.AddDynamic(this, &UMetaGraphicsSettingsWidget::ChangeResolutionY);
	ApplyButton->OnClicked.AddDynamic(this, &UMetaGraphicsSettingsWidget::ApplySettings);
	CloseButton->OnClicked.AddDynamic(this, &UMetaGraphicsSettingsWidget::HideWidget);
	OverallQuality->OnChangedIndex.AddDynamic(this, &UMetaGraphicsSettingsWidget::ChangeOverallQuality);
}

void UMetaGraphicsSettingsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (PlayerController != nullptr)
	{
		PlayerController->GetViewportSize(ViewportSize.X, ViewportSize.Y);
		ViewportSizeTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%dx%d"), ViewportSize.X, ViewportSize.Y)));	
	}

	FVector2D MousePosition;
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
}

void UMetaGraphicsSettingsWidget::InitVariable()
{
	OverallQuality->SetCurrentIndex(MetaToolkitSaveGame->GraphicsSettings.OverallQuality);
	ViewDistanceQuality->SetCurrentIndex(MetaToolkitSaveGame->GraphicsSettings.ViewDistanceQuality);
	GlobalIlluminationQuality->SetCurrentIndex(MetaToolkitSaveGame->GraphicsSettings.GlobalIlluminationQuality);
	ShadowQuality->SetCurrentIndex(MetaToolkitSaveGame->GraphicsSettings.ShadowQuality);
	PostProcessingQuality->SetCurrentIndex(MetaToolkitSaveGame->GraphicsSettings.PostProcessingQuality);
	AntiAliasingQuality->SetCurrentIndex(MetaToolkitSaveGame->GraphicsSettings.AntiAliasingQuality);
	ReflectionQuality->SetCurrentIndex(MetaToolkitSaveGame->GraphicsSettings.ReflectionQuality);
	
	PlayerController->GetViewportSize(ViewportSize.X, ViewportSize.Y);
	
	const FIntPoint Resolution = MetaToolkitSaveGame->GraphicsSettings.Resolution;
	const EWindowMode::Type WindowMode = static_cast<EWindowMode::Type>(MetaToolkitSaveGame->GraphicsSettings.WindowMode);
	
	ResolutionX->SetText(FText::FromString(FString::FromInt(Resolution.X)));
	ResolutionY->SetText(FText::FromString(FString::FromInt(Resolution.Y)));
	switch (WindowMode)
	{
	case EWindowMode::Type::Fullscreen:
		ModeComboBox->SetSelectedOption(TEXT("Full"));
		break;
	case EWindowMode::Type::WindowedFullscreen:
		ModeComboBox->SetSelectedOption(TEXT("Windowed-Full"));
		break;
	case EWindowMode::Type::Windowed:
		ModeComboBox->SetSelectedOption(TEXT("Windowed"));
		break;
	default:
		break;
	}
}

void UMetaGraphicsSettingsWidget::ChangeResolutionX(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (FCString::Atoi(*Text.ToString()) <= 0) ResolutionX->SetText(FText::FromString(TEXT("1920")));
}

void UMetaGraphicsSettingsWidget::ChangeResolutionY(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (FCString::Atoi(*Text.ToString()) <= 0) ResolutionY->SetText(FText::FromString(TEXT("1080")));
}

void UMetaGraphicsSettingsWidget::ChangeOverallQuality(const int32 Quality)
{
	OverallQuality->SetCurrentIndex(Quality);
	ViewDistanceQuality->SetCurrentIndex(Quality);
	GlobalIlluminationQuality->SetCurrentIndex(Quality);
	ShadowQuality->SetCurrentIndex(Quality);
	PostProcessingQuality->SetCurrentIndex(Quality);
	AntiAliasingQuality->SetCurrentIndex(Quality);
	ReflectionQuality->SetCurrentIndex(Quality);
}

void UMetaGraphicsSettingsWidget::ShowWidget()
{
	InitVariable();
	PlayerController->SetInputMode(FInputModeUIOnly());
	PlayerController->SetShowMouseCursor(true);
	SetVisibility(ESlateVisibility::Visible);
}

void UMetaGraphicsSettingsWidget::HideWidget()
{
	PlayerController->SetInputMode(FInputModeGameOnly());
	PlayerController->SetShowMouseCursor(false);
	SetVisibility(ESlateVisibility::Collapsed);
}

void UMetaGraphicsSettingsWidget::ApplySettings()
{
	ApplyResolutionSettings();
	ApplyGraphicsSettings();
}

void UMetaGraphicsSettingsWidget::ApplyResolutionSettings()
{
	const FIntPoint Resolution = FIntPoint(FCString::Atoi(*ResolutionX->GetText().ToString()), FCString::Atoi(*ResolutionY->GetText().ToString()));
	const EWindowMode::Type WindowMode = WindowModeMap.FindRef(ModeComboBox->GetSelectedIndex());
	
	if (ViewportSize == Resolution && GameUserSettings->GetFullscreenMode() == WindowMode) return;
	if (GameUserSettings->GetFullscreenMode() == WindowMode &&  WindowMode == EWindowMode::Type::WindowedFullscreen) return;
	
	GameUserSettings->SetScreenResolution(Resolution);
	GameUserSettings->SetFullscreenMode(WindowMode);
	GameUserSettings->ApplyResolutionSettings(false);

	MetaToolkitSaveGame->GraphicsSettings.Resolution = Resolution;
	MetaToolkitSaveGame->GraphicsSettings.WindowMode = static_cast<uint8>(WindowMode);
	MetaToolkitSaveGame->SaveGame();

	UE_LOG(LogTemp, Log, TEXT("Apply resolution settings %s // %s"), *Resolution.ToString(), *ModeComboBox->GetSelectedOption());
}

void UMetaGraphicsSettingsWidget::ApplyGraphicsSettings()
{
	GameUserSettings->SetOverallScalabilityLevel(OverallQuality->GetCurrentIndex());
	GameUserSettings->SetViewDistanceQuality(ViewDistanceQuality->GetCurrentIndex());
	GameUserSettings->SetGlobalIlluminationQuality(GlobalIlluminationQuality->GetCurrentIndex());
	GameUserSettings->SetShadowQuality(ShadowQuality->GetCurrentIndex());
	GameUserSettings->SetPostProcessingQuality(PostProcessingQuality->GetCurrentIndex());
	GameUserSettings->SetAntiAliasingQuality(AntiAliasingQuality->GetCurrentIndex());
	GameUserSettings->SetReflectionQuality(ReflectionQuality->GetCurrentIndex());
	GameUserSettings->ApplySettings(false);

	MetaToolkitSaveGame->GraphicsSettings.OverallQuality = OverallQuality->GetCurrentIndex();
	MetaToolkitSaveGame->GraphicsSettings.ViewDistanceQuality = ViewDistanceQuality->GetCurrentIndex();
	MetaToolkitSaveGame->GraphicsSettings.GlobalIlluminationQuality = GlobalIlluminationQuality->GetCurrentIndex();
	MetaToolkitSaveGame->GraphicsSettings.ShadowQuality = ShadowQuality->GetCurrentIndex();
	MetaToolkitSaveGame->GraphicsSettings.PostProcessingQuality = PostProcessingQuality->GetCurrentIndex();
	MetaToolkitSaveGame->GraphicsSettings.AntiAliasingQuality = AntiAliasingQuality->GetCurrentIndex();
	MetaToolkitSaveGame->GraphicsSettings.ReflectionQuality = ReflectionQuality->GetCurrentIndex();
	MetaToolkitSaveGame->SaveGame();
	
	UE_LOG(LogTemp, Log, TEXT("Apply Graphics settings"));
}