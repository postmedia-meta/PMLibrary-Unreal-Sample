// Fill out your copyright notice in the Description page of Project Settings.


#include "MetaWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "GameFramework/GameUserSettings.h"

UMetaWidget::UMetaWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetCursor(EMouseCursor::Type::None);
}

void UMetaWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MetaCursorCanvasPanelSlot = Cast<UCanvasPanelSlot>(MetaCursor->Slot);
	if (MetaCursorCanvasPanelSlot == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("CursorImage slot is not Canvas Panel"));
		return;
	}

	PlayerController = GetOwningPlayer();
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr"));
		return;
	}

	GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("GameUserSettings is nullptr!!!"));
		return;
	}

	MetaCursor->SetVisibility(ESlateVisibility::HitTestInvisible);

	// init original size
	OriginViewportSize = FVector2D(SizeBox->GetWidthOverride(), SizeBox->GetHeightOverride());
	OriginCursorSize = MetaCursorCanvasPanelSlot->GetSize();

	// init viewport size and scale
	PlayerController->GetViewportSize(ViewportSize.X, ViewportSize.Y);
	SetSizeBoxSize(ViewportSize.X, ViewportSize.Y);
	if (bAutoResize) MetaCursorResizing();

	// init mouse position
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
	MetaCursorCanvasPanelSlot->SetPosition(MousePosition);

	// Event
	FViewport::ViewportResizedEvent.AddUObject(this, &UMetaWidget::MetaNativeOnViewportResized);
}

FReply UMetaWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

void UMetaWidget::MetaNativeOnViewportResized(FViewport* Viewport, unsigned int I)
{
	ViewportSize = Viewport->GetSizeXY();
	SetSizeBoxSize(ViewportSize.X, ViewportSize.Y);
	if (bAutoResize) MetaCursorResizing();
}

void UMetaWidget::SetSizeBoxSize(const int32 Width, const int32 Height)
{
	SizeBox->SetWidthOverride(Width);
	SizeBox->SetHeightOverride(Height);
}

void UMetaWidget::MetaCursorResizing()
{
	if (MetaCursorCanvasPanelSlot)
	{
		const float Ratio = SizeBox->GetWidthOverride() / OriginViewportSize.X;
		if (Ratio == 0) return;
		
		const FVector2D NewSize = OriginCursorSize * Ratio;
		MetaCursorCanvasPanelSlot->SetSize(NewSize);	
	}
}