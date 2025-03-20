// Fill out your copyright notice in the Description page of Project Settings.


#include "MetaWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

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
	}

	PlayerController = GetOwningPlayer();
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr"));
	}

	MetaCursor->SetVisibility(ESlateVisibility::HitTestInvisible);

	PlayerController->GetViewportSize(ViewportSize.X, ViewportSize.Y);
	SetSizeBoxSize(ViewportSize.X, ViewportSize.Y);

	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
	MetaCursorCanvasPanelSlot->SetPosition(MousePosition);

	FViewport::ViewportResizedEvent.AddUObject(this, &UMetaWidget::MetaNativeOnViewportResized);
}

FReply UMetaWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (MetaCursorCanvasPanelSlot)
	{
		PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
		MetaCursorCanvasPanelSlot->SetPosition(MousePosition);
	}
	
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void UMetaWidget::MetaNativeOnViewportResized(FViewport* Viewport, unsigned int I)
{
	ViewportSize = Viewport->GetSizeXY();
	SetSizeBoxSize(ViewportSize.X, ViewportSize.Y);
}

void UMetaWidget::SetSizeBoxSize(const int32 Width, const int32 Height)
{
	SizeBox->SetWidthOverride(Width);
	SizeBox->SetHeightOverride(Height);
}