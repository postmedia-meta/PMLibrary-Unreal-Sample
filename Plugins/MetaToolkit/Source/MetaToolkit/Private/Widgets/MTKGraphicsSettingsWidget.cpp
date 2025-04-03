// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MTKGraphicsSettingsWidget.h"

#include "Widgets/MTKArrowSwitchWidget.h"
#include "MTKSaveGame.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameUserSettings.h"
#include "TimerManager.h"
#include "Camera/CameraActor.h"

void UMTKGraphicsSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MTKSaveGame = UMTKSaveGame::Get();
	if (MTKSaveGame == nullptr)
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

	ResolutionX->OnTextCommitted.AddDynamic(this, &UMTKGraphicsSettingsWidget::ChangeResolutionX);
	ResolutionY->OnTextCommitted.AddDynamic(this, &UMTKGraphicsSettingsWidget::ChangeResolutionY);
	ApplyButton->OnClicked.AddDynamic(this, &UMTKGraphicsSettingsWidget::ApplySettings);
	CloseButton->OnClicked.AddDynamic(this, &UMTKGraphicsSettingsWidget::HideWidget);
	ScreenMessageButton->OnClicked.AddDynamic(this, &UMTKGraphicsSettingsWidget::ToggleScreenMessage);
	OverallQuality->OnChangedIndex.AddDynamic(this, &UMTKGraphicsSettingsWidget::ChangeOverallQuality);

	Setup();
}

void UMTKGraphicsSettingsWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (InitTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(InitTimerHandle);
		InitTimerHandle.Invalidate();
	}
}

void UMTKGraphicsSettingsWidget::MetaNativeOnViewportResized(FViewport* Viewport, unsigned int I)
{
	Super::MetaNativeOnViewportResized(Viewport, I);
	
	if (PlayerController != nullptr)
	{
		ViewportSizeTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%dx%d"), ViewportSize.X, ViewportSize.Y)));	
	}
}

void UMTKGraphicsSettingsWidget::Setup()
{
	if (ViewportSize.X > 0 && ViewportSize.Y > 0)
	{
		ViewportSizeTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%dx%d"), ViewportSize.X, ViewportSize.Y)));
		OriginMouseCaptureMode = GetWorld()->GetGameViewport()->GetMouseCaptureMode();
		OriginMouseLockMode = GetWorld()->GetGameViewport()->GetMouseLockMode();
		
		InitVariable();
		ApplySettings();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &UMTKGraphicsSettingsWidget::Setup, 0.1f, false);
	}
}

void UMTKGraphicsSettingsWidget::InitVariable()
{
	// Screen Message
	GAreScreenMessagesEnabled = MTKSaveGame->GraphicsSettings.bEnabledScreenMessage;
	ScreenMessageStateTextBlock->SetText(GAreScreenMessagesEnabled ? FText::FromString(TEXT("Show")) : FText::FromString(TEXT("Hide")));
	
	// Quality setting
	OverallQuality->SetCurrentIndex(MTKSaveGame->GraphicsSettings.OverallQuality);
	ViewDistanceQuality->SetCurrentIndex(MTKSaveGame->GraphicsSettings.ViewDistanceQuality);
	GlobalIlluminationQuality->SetCurrentIndex(MTKSaveGame->GraphicsSettings.GlobalIlluminationQuality);
	ShadowQuality->SetCurrentIndex(MTKSaveGame->GraphicsSettings.ShadowQuality);
	PostProcessingQuality->SetCurrentIndex(MTKSaveGame->GraphicsSettings.PostProcessingQuality);
	AntiAliasingQuality->SetCurrentIndex(MTKSaveGame->GraphicsSettings.AntiAliasingQuality);
	ReflectionQuality->SetCurrentIndex(MTKSaveGame->GraphicsSettings.ReflectionQuality);

	// Get save viewport size
	const FIntPoint Resolution = MTKSaveGame->GraphicsSettings.Resolution;
	const EWindowMode::Type WindowMode = static_cast<EWindowMode::Type>(MTKSaveGame->GraphicsSettings.WindowMode);

	// Viewport setting
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

void UMTKGraphicsSettingsWidget::ChangeResolutionX(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (FCString::Atoi(*Text.ToString()) <= 0) ResolutionX->SetText(FText::FromString(TEXT("1920")));
}

void UMTKGraphicsSettingsWidget::ChangeResolutionY(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (FCString::Atoi(*Text.ToString()) <= 0) ResolutionY->SetText(FText::FromString(TEXT("1080")));
}

void UMTKGraphicsSettingsWidget::ChangeOverallQuality(const int32 Quality)
{
	ViewDistanceQuality->SetCurrentIndex(Quality);
	GlobalIlluminationQuality->SetCurrentIndex(Quality);
	ShadowQuality->SetCurrentIndex(Quality);
	PostProcessingQuality->SetCurrentIndex(Quality);
	AntiAliasingQuality->SetCurrentIndex(Quality);
	ReflectionQuality->SetCurrentIndex(Quality);
}

void UMTKGraphicsSettingsWidget::ShowWidget()
{
	InitVariable();
	PlayerController->SetInputMode(FInputModeGameAndUI());
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UMTKGraphicsSettingsWidget::HideWidget()
{
	if (ViewportSize.X > 0 && ViewportSize.Y > 0)
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		GetWorld()->GetGameViewport()->SetMouseCaptureMode(OriginMouseCaptureMode);
		GetWorld()->GetGameViewport()->SetMouseLockMode(OriginMouseLockMode);	
	}
	SetVisibility(ESlateVisibility::Collapsed);
}

void UMTKGraphicsSettingsWidget::ToggleScreenMessage()
{
	GAreScreenMessagesEnabled = !GAreScreenMessagesEnabled;
	ScreenMessageStateTextBlock->SetText(GAreScreenMessagesEnabled ? FText::FromString(TEXT("Show")) : FText::FromString(TEXT("Hide")));
}

void UMTKGraphicsSettingsWidget::ApplySettings()
{
	MTKSaveGame->GraphicsSettings.bEnabledScreenMessage = GAreScreenMessagesEnabled;
	MTKSaveGame->SaveGame();
	
	ApplyResolutionSettings();
	ApplyGraphicsSettings();
}

void UMTKGraphicsSettingsWidget::ApplyResolutionSettings()
{
	const FIntPoint Resolution = FIntPoint(FCString::Atoi(*ResolutionX->GetText().ToString()), FCString::Atoi(*ResolutionY->GetText().ToString()));
	const EWindowMode::Type WindowMode = WindowModeMap.FindRef(ModeComboBox->GetSelectedIndex());

	if (ViewportSize == Resolution && GameUserSettings->GetFullscreenMode() == WindowMode) return;
	if (GameUserSettings->GetFullscreenMode() == WindowMode &&  WindowMode == EWindowMode::Type::WindowedFullscreen) return;
	
	GameUserSettings->SetScreenResolution(Resolution);
	GameUserSettings->SetFullscreenMode(WindowMode);
	GameUserSettings->ApplyResolutionSettings(false);

	MTKSaveGame->GraphicsSettings.Resolution = Resolution;
	MTKSaveGame->GraphicsSettings.WindowMode = static_cast<uint8>(WindowMode);
	MTKSaveGame->SaveGame();

	UE_LOG(LogTemp, Log, TEXT("Apply resolution settings %s // %s"), *Resolution.ToString(), *ModeComboBox->GetSelectedOption());
}

void UMTKGraphicsSettingsWidget::ApplyGraphicsSettings()
{
	GameUserSettings->SetOverallScalabilityLevel(OverallQuality->GetCurrentIndex());
	GameUserSettings->SetViewDistanceQuality(ViewDistanceQuality->GetCurrentIndex());
	GameUserSettings->SetGlobalIlluminationQuality(GlobalIlluminationQuality->GetCurrentIndex());
	GameUserSettings->SetShadowQuality(ShadowQuality->GetCurrentIndex());
	GameUserSettings->SetPostProcessingQuality(PostProcessingQuality->GetCurrentIndex());
	GameUserSettings->SetAntiAliasingQuality(AntiAliasingQuality->GetCurrentIndex());
	GameUserSettings->SetReflectionQuality(ReflectionQuality->GetCurrentIndex());
	GameUserSettings->ApplySettings(false);

	MTKSaveGame->GraphicsSettings.OverallQuality = OverallQuality->GetCurrentIndex();
	MTKSaveGame->GraphicsSettings.ViewDistanceQuality = ViewDistanceQuality->GetCurrentIndex();
	MTKSaveGame->GraphicsSettings.GlobalIlluminationQuality = GlobalIlluminationQuality->GetCurrentIndex();
	MTKSaveGame->GraphicsSettings.ShadowQuality = ShadowQuality->GetCurrentIndex();
	MTKSaveGame->GraphicsSettings.PostProcessingQuality = PostProcessingQuality->GetCurrentIndex();
	MTKSaveGame->GraphicsSettings.AntiAliasingQuality = AntiAliasingQuality->GetCurrentIndex();
	MTKSaveGame->GraphicsSettings.ReflectionQuality = ReflectionQuality->GetCurrentIndex();
	MTKSaveGame->SaveGame();
	
	UE_LOG(LogTemp, Log, TEXT("Apply Graphics settings"));
}