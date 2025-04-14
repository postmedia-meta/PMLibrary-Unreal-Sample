// Fill out your copyright notice in the Description page of Project Settings.


#include "MagicTrail/MetaMagicTrailWidget.h"

#include "MagicTrail/MetaMagicTrailManager.h"
#include "MagicTrail/MetaMagicTrailSaveGame.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableText.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "Engine/GameViewportClient.h"

#if PLATFORM_WINDOWS
#include <windows.h>
#endif

void UMetaMagicTrailWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MetaMagicTrailSaveGame = UMetaMagicTrailSaveGame::Get();
	if (MetaMagicTrailSaveGame == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("SaveGame is nullptr!!!"));
		return;
	}

	PlayerController = GetOwningPlayer();
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr!!!"));
		return;
	}

	MetaMagicTrailComponent = Cast<UMetaMagicTrailManager>(PlayerController->GetComponentByClass(UMetaMagicTrailManager::StaticClass()));
	if (MetaMagicTrailComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("MetaMagicTrailComponent is nullptr!!!"));
		return;
	}

	MetaCursorCanvasPanelSlot = Cast<UCanvasPanelSlot>(MetaCursor->Slot);
	if (MetaCursorCanvasPanelSlot == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("CursorImage slot is not Canvas Panel"));
		return;
	}

	OriginViewportSize = FVector2D(SizeBox->GetWidthOverride(), SizeBox->GetHeightOverride());
	OriginCursorSize = MetaCursorCanvasPanelSlot->GetSize();
	OriginMouseCaptureMode = GetWorld()->GetGameViewport()->GetMouseCaptureMode();
	OriginMouseLockMode = GetWorld()->GetGameViewport()->GetMouseLockMode();

	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
	MetaCursorCanvasPanelSlot->SetPosition(MousePosition);

	InitVariable();
	
	EdgeColorR->OnTextCommitted.AddDynamic(this, &UMetaMagicTrailWidget::OnChangedEdgeColorR);
	EdgeColorG->OnTextCommitted.AddDynamic(this, &UMetaMagicTrailWidget::OnChangedEdgeColorG);
	EdgeColorB->OnTextCommitted.AddDynamic(this, &UMetaMagicTrailWidget::OnChangedEdgeColorB);
	SpriteColorR->OnTextCommitted.AddDynamic(this, &UMetaMagicTrailWidget::OnChangedSpriteColorR);
	SpriteColorG->OnTextCommitted.AddDynamic(this, &UMetaMagicTrailWidget::OnChangedSpriteColorG);
	SpriteColorB->OnTextCommitted.AddDynamic(this, &UMetaMagicTrailWidget::OnChangedSpriteColorB);

	UseFileCheckBox->OnCheckStateChanged.AddDynamic(this, &UMetaMagicTrailWidget::OnChangedUseFile);
	
	ApplyButton->OnClicked.AddDynamic(this, &UMetaMagicTrailWidget::ApplySettings);
	CloseButton->OnClicked.AddDynamic(this, &UMetaMagicTrailWidget::HideWidget);
	SelectedFileButton->OnClicked.AddDynamic(this, &UMetaMagicTrailWidget::OnOpenFileButton);
	ClearFileButton->OnClicked.AddDynamic(this, &UMetaMagicTrailWidget::OnClearFileButton);
}

void UMetaMagicTrailWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	EdgeColorR->OnTextCommitted.Clear();
	EdgeColorG->OnTextCommitted.Clear();
	EdgeColorB->OnTextCommitted.Clear();
	SpriteColorR->OnTextCommitted.Clear();
	SpriteColorG->OnTextCommitted.Clear();
	SpriteColorB->OnTextCommitted.Clear();

	MaskShapeComboBox->OnSelectionChanged.Clear();
	
	ApplyButton->OnClicked.Clear();
	CloseButton->OnClicked.Clear();
}

FReply UMetaMagicTrailWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (PlayerController && MetaCursorCanvasPanelSlot)
	{
		if (PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
		{
			MetaCursorCanvasPanelSlot->SetPosition(MousePosition);	
		}
	}
	
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void UMetaMagicTrailWidget::InitVariable()
{
	MetaMagicTrailComponent->InitCreatePoolNum = MetaMagicTrailSaveGame->MetaMagicTrailData.InitCreatePoolNum;
	MetaMagicTrailComponent->RateScale = MetaMagicTrailSaveGame->MetaMagicTrailData.RateScale;
	MetaMagicTrailComponent->SpriteRateScale = MetaMagicTrailSaveGame->MetaMagicTrailData.SpriteRateScale;
	MetaMagicTrailComponent->LifeTime = MetaMagicTrailSaveGame->MetaMagicTrailData.LifeTime;
	MetaMagicTrailComponent->Scale = MetaMagicTrailSaveGame->MetaMagicTrailData.Scale;
	MetaMagicTrailComponent->SpriteScale = MetaMagicTrailSaveGame->MetaMagicTrailData.SpriteScale;
	MetaMagicTrailComponent->EdgeThickness = MetaMagicTrailSaveGame->MetaMagicTrailData.EdgeThickness;
	MetaMagicTrailComponent->EdgeIntensity = MetaMagicTrailSaveGame->MetaMagicTrailData.EdgeIntensity;
	MetaMagicTrailComponent->ParticleActivationThresholdSec = MetaMagicTrailSaveGame->MetaMagicTrailData.ParticleActivationThresholdSec;
	MetaMagicTrailComponent->ResetTime = MetaMagicTrailSaveGame->MetaMagicTrailData.ResetTime;
	MetaMagicTrailComponent->EdgeColor = MetaMagicTrailSaveGame->MetaMagicTrailData.EdgeColor;
	MetaMagicTrailComponent->SpriteColor = MetaMagicTrailSaveGame->MetaMagicTrailData.SpriteColor;
	MetaMagicTrailComponent->bAutoColor = MetaMagicTrailSaveGame->MetaMagicTrailData.bAutoColor;
	MetaMagicTrailComponent->bUseMouse = MetaMagicTrailSaveGame->MetaMagicTrailData.bUseMouse;
	MetaMagicTrailComponent->bUseFile = MetaMagicTrailSaveGame->MetaMagicTrailData.bUseFile;
	MetaMagicTrailComponent->AssetPath.FilePath = MetaMagicTrailSaveGame->MetaMagicTrailData.FilePath;

	InitCreatePoolNum->SetText(FText::FromString(FString::FromInt(MetaMagicTrailSaveGame->MetaMagicTrailData.InitCreatePoolNum)));
	RateScale->SetText(FText::FromString(FString::SanitizeFloat(MetaMagicTrailSaveGame->MetaMagicTrailData.RateScale)));
	SpriteRateScale->SetText(FText::FromString(FString::SanitizeFloat(MetaMagicTrailSaveGame->MetaMagicTrailData.SpriteRateScale)));
	LifeTime->SetText(FText::FromString(FString::SanitizeFloat(MetaMagicTrailSaveGame->MetaMagicTrailData.LifeTime)));
	Scale->SetText(FText::FromString(FString::SanitizeFloat(MetaMagicTrailSaveGame->MetaMagicTrailData.Scale)));
	SpriteScale->SetText(FText::FromString(FString::SanitizeFloat(MetaMagicTrailSaveGame->MetaMagicTrailData.SpriteScale)));
	EdgeThickness->SetText(FText::FromString(FString::SanitizeFloat(MetaMagicTrailSaveGame->MetaMagicTrailData.EdgeThickness)));
	EdgeIntensity->SetText(FText::FromString(FString::SanitizeFloat(MetaMagicTrailSaveGame->MetaMagicTrailData.EdgeIntensity)));
	ParticleActivationThresholdSec->SetText(FText::FromString(FString::SanitizeFloat(MetaMagicTrailSaveGame->MetaMagicTrailData.ParticleActivationThresholdSec)));
	ResetTime->SetText(FText::FromString(FString::SanitizeFloat(MetaMagicTrailSaveGame->MetaMagicTrailData.ResetTime)));
	EdgeColorR->SetText(FText::FromString(FString::FromInt(MetaMagicTrailSaveGame->MetaMagicTrailData.EdgeColor.R)));
	EdgeColorG->SetText(FText::FromString(FString::FromInt(MetaMagicTrailSaveGame->MetaMagicTrailData.EdgeColor.G)));
	EdgeColorB->SetText(FText::FromString(FString::FromInt(MetaMagicTrailSaveGame->MetaMagicTrailData.EdgeColor.B)));
	SpriteColorR->SetText(FText::FromString(FString::FromInt(MetaMagicTrailSaveGame->MetaMagicTrailData.SpriteColor.R)));
	SpriteColorG->SetText(FText::FromString(FString::FromInt(MetaMagicTrailSaveGame->MetaMagicTrailData.SpriteColor.G)));
	SpriteColorB->SetText(FText::FromString(FString::FromInt(MetaMagicTrailSaveGame->MetaMagicTrailData.SpriteColor.B)));
	AutoColorCheckBox->SetIsChecked(MetaMagicTrailSaveGame->MetaMagicTrailData.bAutoColor);
	UseMouseCheckBox->SetIsChecked(MetaMagicTrailSaveGame->MetaMagicTrailData.bUseMouse);
	UseFileCheckBox->SetIsChecked(MetaMagicTrailSaveGame->MetaMagicTrailData.bUseFile);
	SelectedFilePathTextBlock->SetText(FText::FromString(MetaMagicTrailSaveGame->MetaMagicTrailData.FilePath));

	ClearFileButton->SetIsEnabled(MetaMagicTrailSaveGame->MetaMagicTrailData.bUseFile);
	SelectedFileButton->SetIsEnabled(MetaMagicTrailSaveGame->MetaMagicTrailData.bUseFile);
	
	if (!UseFileCheckBox->IsChecked() || SelectedFilePathTextBlock->GetText().IsEmpty()) SelectedFilePathTextBlock->SetText(FText::FromString(FileEmptyString));
	
	switch (MetaMagicTrailSaveGame->MetaMagicTrailData.MaskShape)
	{
	case EMaskShape::Default:
		MetaMagicTrailComponent->ChangeMaskShape(EMaskShape::Default);
		MaskShapeComboBox->SetSelectedOption(TEXT("Default"));
		break;
	case EMaskShape::Circle:
		MetaMagicTrailComponent->ChangeMaskShape(EMaskShape::Circle);
		MaskShapeComboBox->SetSelectedOption(TEXT("Circle"));
		break;
	case EMaskShape::Square:
		MetaMagicTrailComponent->ChangeMaskShape(EMaskShape::Square);
		MaskShapeComboBox->SetSelectedOption(TEXT("Square"));
		break;
	}
}

void UMetaMagicTrailWidget::ShowWidget()
{
	InitVariable();
	PrevHideCursorDuringCapture = GetWorld()->GetGameViewport()->HideCursorDuringCapture();
	PlayerController->SetInputMode(FInputModeGameAndUI());
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UMetaMagicTrailWidget::HideWidget()
{
	PlayerController->SetInputMode(FInputModeGameOnly());
	GetWorld()->GetGameViewport()->SetHideCursorDuringCapture(PrevHideCursorDuringCapture);
	GetWorld()->GetGameViewport()->SetMouseCaptureMode(OriginMouseCaptureMode);
	GetWorld()->GetGameViewport()->SetMouseLockMode(OriginMouseLockMode);
	SetVisibility(ESlateVisibility::Collapsed);
}

void UMetaMagicTrailWidget::ApplySettings()
{
	MetaMagicTrailComponent->InitCreatePoolNum = FCString::Atoi(*InitCreatePoolNum->GetText().ToString());
	MetaMagicTrailComponent->RateScale = FCString::Atof(*RateScale->GetText().ToString());
	MetaMagicTrailComponent->SpriteRateScale = FCString::Atof(*SpriteRateScale->GetText().ToString());
	MetaMagicTrailComponent->LifeTime = FCString::Atof(*LifeTime->GetText().ToString());
	MetaMagicTrailComponent->Scale = FCString::Atof(*Scale->GetText().ToString());
	MetaMagicTrailComponent->SpriteScale = FCString::Atof(*SpriteScale->GetText().ToString());
	MetaMagicTrailComponent->EdgeThickness = FCString::Atof(*EdgeThickness->GetText().ToString());
	MetaMagicTrailComponent->EdgeIntensity = FCString::Atof(*EdgeIntensity->GetText().ToString());
	MetaMagicTrailComponent->ParticleActivationThresholdSec = FCString::Atof(*ParticleActivationThresholdSec->GetText().ToString());
	MetaMagicTrailComponent->ResetTime = FCString::Atof(*ResetTime->GetText().ToString());
	MetaMagicTrailComponent->EdgeColor = FColor(FCString::Atof(*EdgeColorR->GetText().ToString()), FCString::Atof(*EdgeColorG->GetText().ToString()), FCString::Atof(*EdgeColorB->GetText().ToString()));
	MetaMagicTrailComponent->SpriteColor = FColor(FCString::Atof(*SpriteColorR->GetText().ToString()), FCString::Atof(*SpriteColorG->GetText().ToString()), FCString::Atof(*SpriteColorB->GetText().ToString()));
	MetaMagicTrailComponent->bAutoColor = AutoColorCheckBox->IsChecked();
	MetaMagicTrailComponent->bUseMouse = UseMouseCheckBox->IsChecked();
	MetaMagicTrailComponent->bUseFile = UseFileCheckBox->IsChecked();
	MetaMagicTrailComponent->SetFilePath(SelectedFilePathTextBlock->GetText().ToString());
	
	if (MaskShapeComboBox->GetSelectedOption().Equals(TEXT("Default")))
	{
		MetaMagicTrailComponent->ChangeMaskShape(EMaskShape::Default);
	}
	else if (MaskShapeComboBox->GetSelectedOption().Equals(TEXT("Circle")))
	{
		MetaMagicTrailComponent->ChangeMaskShape(EMaskShape::Circle);
	}
	else if (MaskShapeComboBox->GetSelectedOption().Equals(TEXT("Square")))
	{
		MetaMagicTrailComponent->ChangeMaskShape(EMaskShape::Square);
	}

	MetaMagicTrailSaveGame->MetaMagicTrailData.InitCreatePoolNum = MetaMagicTrailComponent->InitCreatePoolNum;
	MetaMagicTrailSaveGame->MetaMagicTrailData.RateScale = MetaMagicTrailComponent->RateScale;
	MetaMagicTrailSaveGame->MetaMagicTrailData.SpriteRateScale = MetaMagicTrailComponent->SpriteRateScale;
	MetaMagicTrailSaveGame->MetaMagicTrailData.LifeTime = MetaMagicTrailComponent->LifeTime;
	MetaMagicTrailSaveGame->MetaMagicTrailData.Scale = MetaMagicTrailComponent->Scale;
	MetaMagicTrailSaveGame->MetaMagicTrailData.SpriteScale = MetaMagicTrailComponent->SpriteScale;
	MetaMagicTrailSaveGame->MetaMagicTrailData.EdgeThickness = MetaMagicTrailComponent->EdgeThickness;
	MetaMagicTrailSaveGame->MetaMagicTrailData.EdgeIntensity = MetaMagicTrailComponent->EdgeIntensity;
	MetaMagicTrailSaveGame->MetaMagicTrailData.ParticleActivationThresholdSec = MetaMagicTrailComponent->ParticleActivationThresholdSec;
	MetaMagicTrailSaveGame->MetaMagicTrailData.ResetTime = MetaMagicTrailComponent->ResetTime;
	MetaMagicTrailSaveGame->MetaMagicTrailData.EdgeColor = MetaMagicTrailComponent->EdgeColor;
	MetaMagicTrailSaveGame->MetaMagicTrailData.SpriteColor = MetaMagicTrailComponent->SpriteColor;
	MetaMagicTrailSaveGame->MetaMagicTrailData.bAutoColor = MetaMagicTrailComponent->bAutoColor;
	MetaMagicTrailSaveGame->MetaMagicTrailData.bUseMouse = MetaMagicTrailComponent->bUseMouse;
	MetaMagicTrailSaveGame->MetaMagicTrailData.bUseFile = MetaMagicTrailComponent->bUseFile;
	MetaMagicTrailSaveGame->MetaMagicTrailData.MaskShape = MetaMagicTrailComponent->MaskShape;
	MetaMagicTrailSaveGame->MetaMagicTrailData.FilePath = MetaMagicTrailComponent->AssetPath.FilePath;
	MetaMagicTrailSaveGame->SaveGame();
}

void UMetaMagicTrailWidget::SetSizeBoxSize(const int32 Width, const int32 Height)
{
	SizeBox->SetWidthOverride(Width);
	SizeBox->SetHeightOverride(Height);
}

void UMetaMagicTrailWidget::MetaCursorResizing()
{
	if (MetaCursorCanvasPanelSlot)
	{
		const float Ratio = SizeBox->GetWidthOverride() / OriginViewportSize.X;
		if (Ratio == 0) return;
		
		const FVector2D NewSize = OriginCursorSize * Ratio;
		MetaCursorCanvasPanelSlot->SetSize(NewSize);	
	}
}

void UMetaMagicTrailWidget::OnChangedEdgeColorR(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (FCString::Atof(*Text.ToString()) > 255) { EdgeColorR->SetText(FText::FromString(TEXT("255"))); }
	if (FCString::Atof(*Text.ToString()) < 0) { EdgeColorR->SetText(FText::FromString(TEXT("0"))); }
}

void UMetaMagicTrailWidget::OnChangedEdgeColorG(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (FCString::Atof(*Text.ToString()) > 255) { EdgeColorG->SetText(FText::FromString(TEXT("255"))); }
	if (FCString::Atof(*Text.ToString()) < 0) { EdgeColorG->SetText(FText::FromString(TEXT("0"))); }
}

void UMetaMagicTrailWidget::OnChangedEdgeColorB(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (FCString::Atof(*Text.ToString()) > 255) { EdgeColorB->SetText(FText::FromString(TEXT("255"))); }
	if (FCString::Atof(*Text.ToString()) < 0) { EdgeColorB->SetText(FText::FromString(TEXT("0"))); }
}

void UMetaMagicTrailWidget::OnChangedSpriteColorR(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (FCString::Atof(*Text.ToString()) > 255) { SpriteColorR->SetText(FText::FromString(TEXT("255"))); }
	if (FCString::Atof(*Text.ToString()) < 0) { SpriteColorR->SetText(FText::FromString(TEXT("0"))); }
}

void UMetaMagicTrailWidget::OnChangedSpriteColorG(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (FCString::Atof(*Text.ToString()) > 255) { SpriteColorG->SetText(FText::FromString(TEXT("255"))); }
	if (FCString::Atof(*Text.ToString()) < 0) { SpriteColorG->SetText(FText::FromString(TEXT("0"))); }
}

void UMetaMagicTrailWidget::OnChangedSpriteColorB(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (FCString::Atof(*Text.ToString()) > 255) { SpriteColorB->SetText(FText::FromString(TEXT("255"))); }
	if (FCString::Atof(*Text.ToString()) < 0) { SpriteColorB->SetText(FText::FromString(TEXT("0"))); }
}

void UMetaMagicTrailWidget::OnOpenFileButton()
{
#if PLATFORM_WINDOWS
	const int32 BufferSize = 260;
	TCHAR szFile[BufferSize] = { 0 };
	ZeroMemory(&szFile, sizeof(szFile));
	
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = BufferSize / sizeof(TCHAR);
	ofn.lpstrFilter = TEXT("Video Files (*.mp4)\0*.mp4\0Image Files (*.jpg;*.jpeg;*.png;*.bmp;*.tga;*.psd;*.exr)\0*.jpg;*.jpeg;*.png;*.bmp;*.tga;*.psd;*.exr\0");
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
	// 파일 다이얼로그 열기
	if (GetOpenFileName(&ofn))
	{
		SelectedFilePathTextBlock->SetText(FText::FromString(ofn.lpstrFile));
		UE_LOG(LogTemp, Log, TEXT("Selected File: %s"), ofn.lpstrFile);	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("File selection was canceled or failed."));
	}
#else
	UE_LOG(LogTemp, Warning, TEXT("The file dialog feature is not implemented on the current platform."));
#endif
}

void UMetaMagicTrailWidget::OnClearFileButton()
{
	SelectedFilePathTextBlock->SetText(FText::FromString(FileEmptyString));
}

void UMetaMagicTrailWidget::OnChangedUseFile(bool bIsChecked)
{
	ClearFileButton->SetIsEnabled(bIsChecked);
	SelectedFileButton->SetIsEnabled(bIsChecked);
}
