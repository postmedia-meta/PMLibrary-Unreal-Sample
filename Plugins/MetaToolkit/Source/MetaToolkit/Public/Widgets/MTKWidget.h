// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MTKWidget.generated.h"

class UScaleBox;
class USizeBox;
class UCanvasPanel;
class UImage;
class UCanvasPanelSlot;
class APlayerController;
class UGameUserSettings;
class FViewport;

UCLASS(meta=(DisableNativeTick))
class METATOOLKIT_API UMTKWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMTKWidget(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaWidget")
	UScaleBox* ScaleBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaWidget")
	USizeBox* SizeBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaWidget")
	UCanvasPanel* CanvasPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaWidget")
	UImage* MetaCursor;

protected:
	UPROPERTY(BlueprintReadWrite, Category="MetaWidget")
	FIntPoint ViewportSize = FIntPoint(0, 0);

	UPROPERTY(BlueprintReadWrite, Category="MetaWidget|Mouse")
	FVector2D MousePosition = FVector2D(0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaWidget|Mouse")
	bool bAutoResize = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PlayerController")
	APlayerController* PlayerController = nullptr;

	UPROPERTY()
	UGameUserSettings* GameUserSettings = nullptr;

private:
	UPROPERTY()
	FVector2D OriginViewportSize = FVector2D(0, 0);

	UPROPERTY()
	FVector2D OriginCursorSize = FVector2D(0, 0);
	
	UPROPERTY()
	UCanvasPanelSlot* MetaCursorCanvasPanelSlot = nullptr;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void MetaNativeOnViewportResized(FViewport* Viewport, unsigned int I);

public:
	UFUNCTION(BlueprintCallable, Category="MetaWidget")
	void SetSizeBoxSize(const int32 Width, const int32 Height);

	UFUNCTION(BlueprintCallable, Category="MetaWidget")
	void MetaCursorResizing();
};
