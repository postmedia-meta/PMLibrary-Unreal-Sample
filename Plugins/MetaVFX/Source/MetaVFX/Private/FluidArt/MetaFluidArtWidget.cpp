// Fill out your copyright notice in the Description page of Project Settings.


#include "FluidArt/MetaFluidArtWidget.h"

#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableText.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "FluidArt/MetaFluidArtManager.h"
#include "FluidArt/MetaFluidArtSaveGame.h"
#include "Engine/GameViewportClient.h"

#if PLATFORM_WINDOWS
#include <windows.h>
#endif

void UMetaFluidArtWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MetaFluidArtSaveGame = UMetaFluidArtSaveGame::Get();
	if (MetaFluidArtSaveGame == nullptr)
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

	MetaFluidArtControlComponent = Cast<UMetaFluidArtManager>(PlayerController->GetComponentByClass(UMetaFluidArtManager::StaticClass()));
	if (MetaFluidArtControlComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("MetaFluidArtControlComponent is nullptr!!!"));
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

	InitCreatePoolNum->OnTextCommitted.AddDynamic(this, &UMetaFluidArtWidget::OnChangedInitCreatePoolNum);
	InteractionScale->OnTextCommitted.AddDynamic(this, &UMetaFluidArtWidget::OnChangedTextureInteractionScale);
	ScaleIncreaseTime->OnTextCommitted.AddDynamic(this, &UMetaFluidArtWidget::OnChangedTextureScaleIncreaseTime);
	ScaleDecreaseTime->OnTextCommitted.AddDynamic(this, &UMetaFluidArtWidget::OnChangedTextureScaleDecreaseTime);

	SpriteScale->OnTextCommitted.AddDynamic(this, &UMetaFluidArtWidget::OnChangedSpriteScale);
	NoiseForce->OnTextCommitted.AddDynamic(this, &UMetaFluidArtWidget::OnChangedNoiseForce);
	NoiseFrequency->OnTextCommitted.AddDynamic(this, &UMetaFluidArtWidget::OnChangedNoiseFrequency);
	TextureChangeDelay->OnTextCommitted.AddDynamic(this, &UMetaFluidArtWidget::OnChangedTextureChangeDelay);
	NumCellsMaxAxis->OnTextCommitted.AddDynamic(this, &UMetaFluidArtWidget::OnChangedNumCellsMaxAxis);
	ParticlePerCell->OnTextCommitted.AddDynamic(this, &UMetaFluidArtWidget::OnChangedParticlePerCell);
	PressureIterations->OnTextCommitted.AddDynamic(this, &UMetaFluidArtWidget::OnChangedPressureIterations);
	UVStepU->OnTextCommitted.AddDynamic(this, &UMetaFluidArtWidget::OnChangedUVStepU);
	UVStepV->OnTextCommitted.AddDynamic(this, &UMetaFluidArtWidget::OnChangedUVStepV);

	ApplyButton->OnClicked.AddDynamic(this, &UMetaFluidArtWidget::ApplySettings);
	CloseButton->OnClicked.AddDynamic(this, &UMetaFluidArtWidget::HideWidget);
	SelectedPathButton->OnClicked.AddDynamic(this, &UMetaFluidArtWidget::OnOpenFileButton);
	ClearPathsButton->OnClicked.AddDynamic(this, &UMetaFluidArtWidget::OnClearFileButton);

	PathsComboBox->OnSelectionChanged.AddDynamic(this, &UMetaFluidArtWidget::OnPathSelectionChanged);
}

void UMetaFluidArtWidget::NativeDestruct()
{
	Super::NativeDestruct();

	InitCreatePoolNum->OnTextCommitted.RemoveDynamic(this, &UMetaFluidArtWidget::OnChangedInitCreatePoolNum);
	InteractionScale->OnTextCommitted.RemoveDynamic(this, &UMetaFluidArtWidget::OnChangedTextureInteractionScale);
	ScaleIncreaseTime->OnTextCommitted.RemoveDynamic(this, &UMetaFluidArtWidget::OnChangedTextureScaleIncreaseTime);
	ScaleDecreaseTime->OnTextCommitted.RemoveDynamic(this, &UMetaFluidArtWidget::OnChangedTextureScaleDecreaseTime);

	SpriteScale->OnTextCommitted.RemoveDynamic(this, &UMetaFluidArtWidget::OnChangedSpriteScale);
	TextureChangeDelay->OnTextCommitted.RemoveDynamic(this, &UMetaFluidArtWidget::OnChangedTextureChangeDelay);
	NumCellsMaxAxis->OnTextCommitted.RemoveDynamic(this, &UMetaFluidArtWidget::OnChangedNumCellsMaxAxis);
	ParticlePerCell->OnTextCommitted.RemoveDynamic(this, &UMetaFluidArtWidget::OnChangedParticlePerCell);
	PressureIterations->OnTextCommitted.RemoveDynamic(this, &UMetaFluidArtWidget::OnChangedPressureIterations);
	UVStepU->OnTextCommitted.RemoveDynamic(this, &UMetaFluidArtWidget::OnChangedUVStepU);
	UVStepV->OnTextCommitted.RemoveDynamic(this, &UMetaFluidArtWidget::OnChangedUVStepV);

	ApplyButton->OnClicked.RemoveDynamic(this, &UMetaFluidArtWidget::ApplySettings);
	CloseButton->OnClicked.RemoveDynamic(this, &UMetaFluidArtWidget::HideWidget);
	SelectedPathButton->OnClicked.RemoveDynamic(this, &UMetaFluidArtWidget::OnOpenFileButton);
	ClearPathsButton->OnClicked.RemoveDynamic(this, &UMetaFluidArtWidget::OnClearFileButton);

	PathsComboBox->OnSelectionChanged.RemoveDynamic(this, &UMetaFluidArtWidget::OnPathSelectionChanged);
}

FReply UMetaFluidArtWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

void UMetaFluidArtWidget::InitVariable()
{
	PathsComboBox->ClearSelection();

	// Controller data
	InitCreatePoolNum->SetText(FText::FromString(FString::FromInt(MetaFluidArtSaveGame->ControllerData.InitCreatePoolNum)));
	InteractionScale->SetText(FText::FromString(FString::SanitizeFloat(MetaFluidArtSaveGame->ControllerData.InteractionScale)));
	ScaleIncreaseTime->SetText(FText::FromString(FString::SanitizeFloat(MetaFluidArtSaveGame->ControllerData.ScaleIncreaseTime)));
	ScaleDecreaseTime->SetText(FText::FromString(FString::SanitizeFloat(MetaFluidArtSaveGame->ControllerData.ScaleDecreaseTime)));
	
	MetaFluidArtControlComponent->InitCreatePoolNum = MetaFluidArtSaveGame->ControllerData.InitCreatePoolNum;
	MetaFluidArtControlComponent->InteractionScale = MetaFluidArtSaveGame->ControllerData.InteractionScale;
	MetaFluidArtControlComponent->ScaleIncreaseTime = MetaFluidArtSaveGame->ControllerData.ScaleIncreaseTime;
	MetaFluidArtControlComponent->ScaleDecreaseTime = MetaFluidArtSaveGame->ControllerData.ScaleDecreaseTime;

	// Niagara data
	NumCellsMaxAxis->SetText(FText::FromString(FString::FromInt(MetaFluidArtSaveGame->NiagaraData.NumCellsMaxAxis)));
	ParticlePerCell->SetText(FText::FromString(FString::FromInt(MetaFluidArtSaveGame->NiagaraData.ParticlePerCell)));
	PressureIterations->SetText(FText::FromString(FString::FromInt(MetaFluidArtSaveGame->NiagaraData.PressureIterations)));
	TextureChangeDelay->SetText(FText::FromString(FString::SanitizeFloat(MetaFluidArtSaveGame->NiagaraData.TextureChangeDelay)));
	NoiseForce->SetText(FText::FromString(FString::SanitizeFloat(MetaFluidArtSaveGame->NiagaraData.NoiseForce)));
	NoiseFrequency->SetText(FText::FromString(FString::SanitizeFloat(MetaFluidArtSaveGame->NiagaraData.NoiseFrequency)));
	SpriteScale->SetText(FText::FromString(FString::SanitizeFloat(MetaFluidArtSaveGame->NiagaraData.SpriteScale)));
	UVStepU->SetText(FText::FromString(FString::SanitizeFloat(MetaFluidArtSaveGame->NiagaraData.UVStep.X)));
	UVStepV->SetText(FText::FromString(FString::SanitizeFloat(MetaFluidArtSaveGame->NiagaraData.UVStep.Y)));
	AutoNoiseCheckBox->SetIsChecked(MetaFluidArtSaveGame->NiagaraData.bAutoNoise);
	PathsComboBox->ClearOptions();
	
	for (const FString FilePath : MetaFluidArtSaveGame->NiagaraData.FilePaths)
	{
		PathsComboBox->AddOption(FilePath);
	}
}

void UMetaFluidArtWidget::ShowWidget()
{
	InitVariable();
	PrevHideCursorDuringCapture = GetWorld()->GetGameViewport()->HideCursorDuringCapture();
	PlayerController->SetInputMode(FInputModeGameAndUI());
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UMetaFluidArtWidget::HideWidget()
{
	PlayerController->SetInputMode(FInputModeGameOnly());
	GetWorld()->GetGameViewport()->SetHideCursorDuringCapture(PrevHideCursorDuringCapture);
	GetWorld()->GetGameViewport()->SetMouseCaptureMode(OriginMouseCaptureMode);
	GetWorld()->GetGameViewport()->SetMouseLockMode(OriginMouseLockMode);
	SetVisibility(ESlateVisibility::Collapsed);
}

void UMetaFluidArtWidget::ApplySettings()
{
	// Controller data
	MetaFluidArtSaveGame->ControllerData.InitCreatePoolNum = FCString::Atoi(*InitCreatePoolNum->GetText().ToString());
	MetaFluidArtSaveGame->ControllerData.InteractionScale = FCString::Atof(*InteractionScale->GetText().ToString());
	MetaFluidArtSaveGame->ControllerData.ScaleIncreaseTime = FCString::Atof(*ScaleIncreaseTime->GetText().ToString());
	MetaFluidArtSaveGame->ControllerData.ScaleDecreaseTime = FCString::Atof(*ScaleDecreaseTime->GetText().ToString());
	
	MetaFluidArtControlComponent->InitCreatePoolNum = MetaFluidArtSaveGame->ControllerData.InitCreatePoolNum;
	MetaFluidArtControlComponent->InteractionScale = MetaFluidArtSaveGame->ControllerData.InteractionScale;
	MetaFluidArtControlComponent->ScaleIncreaseTime = MetaFluidArtSaveGame->ControllerData.ScaleIncreaseTime;
	MetaFluidArtControlComponent->ScaleDecreaseTime = MetaFluidArtSaveGame->ControllerData.ScaleDecreaseTime;

	// Niagara data
	MetaFluidArtSaveGame->NiagaraData.NumCellsMaxAxis = FCString::Atoi(*NumCellsMaxAxis->GetText().ToString());
	MetaFluidArtSaveGame->NiagaraData.ParticlePerCell = FCString::Atoi(*ParticlePerCell->GetText().ToString());
	MetaFluidArtSaveGame->NiagaraData.PressureIterations = FCString::Atoi(*PressureIterations->GetText().ToString());
	MetaFluidArtSaveGame->NiagaraData.TextureChangeDelay = FCString::Atof(*TextureChangeDelay->GetText().ToString());
	MetaFluidArtSaveGame->NiagaraData.NumCellsMaxAxis = FCString::Atoi(*NumCellsMaxAxis->GetText().ToString());
	MetaFluidArtSaveGame->NiagaraData.NoiseForce = FCString::Atof(*NoiseForce->GetText().ToString());
	MetaFluidArtSaveGame->NiagaraData.NoiseFrequency = FCString::Atof(*NoiseFrequency->GetText().ToString());
	MetaFluidArtSaveGame->NiagaraData.SpriteScale = FCString::Atof(*SpriteScale->GetText().ToString());
	MetaFluidArtSaveGame->NiagaraData.UVStep = FVector2D(FCString::Atof(*UVStepU->GetText().ToString()), FCString::Atof(*UVStepV->GetText().ToString()));
	MetaFluidArtSaveGame->NiagaraData.bAutoNoise = AutoNoiseCheckBox->IsChecked();
	MetaFluidArtSaveGame->NiagaraData.FilePaths.Empty();
	
	for (int i = 0; i < PathsComboBox->GetOptionCount(); ++i)
	{
		MetaFluidArtSaveGame->NiagaraData.FilePaths.Emplace(PathsComboBox->GetOptionAtIndex(i));
	}
	MetaFluidArtSaveGame->SaveGame();
}

void UMetaFluidArtWidget::SetSizeBoxSize(const int32 Width, const int32 Height)
{
	SizeBox->SetWidthOverride(Width);
	SizeBox->SetHeightOverride(Height);
}

void UMetaFluidArtWidget::MetaCursorResizing()
{
	if (MetaCursorCanvasPanelSlot)
	{
		const float Ratio = SizeBox->GetWidthOverride() / OriginViewportSize.X;
		if (Ratio == 0) return;
		
		const FVector2D NewSize = OriginCursorSize * Ratio;
		MetaCursorCanvasPanelSlot->SetSize(NewSize);	
	}
}

void UMetaFluidArtWidget::OnChangedInitCreatePoolNum(const FText& Text, ETextCommit::Type CommitMethod)
{
	InitCreatePoolNum->SetText(FText::FromString(FString::Printf(TEXT("%d"), FCString::Atoi(*Text.ToString()))));
}

void UMetaFluidArtWidget::OnChangedTextureInteractionScale(const FText& Text, ETextCommit::Type Arg)
{
	InteractionScale->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), FCString::Atof(*Text.ToString()))));
}

void UMetaFluidArtWidget::OnChangedTextureScaleIncreaseTime(const FText& Text, ETextCommit::Type Arg)
{
	ScaleIncreaseTime->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), FCString::Atof(*Text.ToString()))));
}

void UMetaFluidArtWidget::OnChangedTextureScaleDecreaseTime(const FText& Text, ETextCommit::Type Arg)
{
	ScaleDecreaseTime->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), FCString::Atof(*Text.ToString()))));
}

void UMetaFluidArtWidget::OnChangedNoiseForce(const FText& Text, ETextCommit::Type CommitMethod)
{
	NoiseForce->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), FCString::Atof(*Text.ToString()))));
}

void UMetaFluidArtWidget::OnChangedNoiseFrequency(const FText& Text, ETextCommit::Type CommitMethod)
{
	NoiseFrequency->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), FCString::Atof(*Text.ToString()))));
}

void UMetaFluidArtWidget::OnChangedSpriteScale(const FText& Text, ETextCommit::Type CommitMethod)
{
	SpriteScale->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), FCString::Atof(*Text.ToString()))));
}

void UMetaFluidArtWidget::OnChangedTextureChangeDelay(const FText& Text, ETextCommit::Type CommitMethod)
{
	float Delay = FCString::Atof(*Text.ToString());
	if (Delay < 1) Delay = 1;
	
	TextureChangeDelay->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), Delay)));
}

void UMetaFluidArtWidget::OnChangedNumCellsMaxAxis(const FText& Text, ETextCommit::Type CommitMethod)
{
	NumCellsMaxAxis->SetText(FText::FromString(FString::Printf(TEXT("%d"), FCString::Atoi(*Text.ToString()))));
}

void UMetaFluidArtWidget::OnChangedParticlePerCell(const FText& Text, ETextCommit::Type CommitMethod)
{
	int32 Num = FCString::Atoi(*Text.ToString());
	if (Num < 1) Num = 1;
	
	ParticlePerCell->SetText(FText::FromString(FString::Printf(TEXT("%d"), Num)));
}

void UMetaFluidArtWidget::OnChangedPressureIterations(const FText& Text, ETextCommit::Type CommitMethod)
{
	PressureIterations->SetText(FText::FromString(FString::Printf(TEXT("%d"), FCString::Atoi(*Text.ToString()))));
}

void UMetaFluidArtWidget::OnChangedUVStepU(const FText& Text, ETextCommit::Type CommitMethod)
{
	UVStepU->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), FCString::Atof(*Text.ToString()))));
}

void UMetaFluidArtWidget::OnChangedUVStepV(const FText& Text, ETextCommit::Type CommitMethod)
{
	UVStepV->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), FCString::Atof(*Text.ToString()))));
}

void UMetaFluidArtWidget::OnOpenFileButton()
{
#if PLATFORM_WINDOWS
	const int32 BufferSize = 4096;
	TCHAR szFile[BufferSize] = { 0 };
	ZeroMemory(&szFile, sizeof(szFile));
	
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = BufferSize / sizeof(TCHAR);
	ofn.lpstrFilter = TEXT("Image Files (*.jpg;*.jpeg;*.png;*.bmp;*.tga;*.psd;*.exr)\0*.jpg;*.jpeg;*.png;*.bmp;*.tga;*.psd;*.exr\0");
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;
	UE_LOG(LogTemp, Warning, TEXT("Test!!! %s"), szFile);
	// 파일 다이얼로그 열기
	if (GetOpenFileName(&ofn))
	{
		UE_LOG(LogTemp, Log, TEXT("=====<Selected File>====="));

		int32 Counter = 0;
		const FString FirstEntry = FString(ofn.lpstrFile);
		TCHAR* NextFileNamePtr = ofn.lpstrFile + FirstEntry.Len() + 1;
		if (*NextFileNamePtr == '\0')
		{
			++Counter;
			PathsComboBox->AddOption(FirstEntry);
			UE_LOG(LogTemp, Log, TEXT("%s"), *FirstEntry);
		}
		else
		{
			// 여러 파일이 선택된 경우, 첫 번째 문자열은 디렉터리 경로
			const FString Directory = FirstEntry;
			while (*NextFileNamePtr)
			{
				++Counter;
				FString FileName = FString(NextFileNamePtr);
				const FString Path = Directory / FileName;
				PathsComboBox->AddOption(Path);
				UE_LOG(LogTemp, Log, TEXT("%s"), *Path);
				NextFileNamePtr += FileName.Len() + 1;
			}
		}
		
		UE_LOG(LogTemp, Log, TEXT("=====End (File Num: %d)====="), Counter);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("File selection was canceled or failed."));
	}
#else
	UE_LOG(LogTemp, Warning, TEXT("The file dialog feature is not implemented on the current platform."));
#endif
}

void UMetaFluidArtWidget::OnClearFileButton()
{
	PathsComboBox->ClearOptions();
}

void UMetaFluidArtWidget::OnPathSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	PathsComboBox->ClearSelection();
}