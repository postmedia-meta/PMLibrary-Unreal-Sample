// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MetaWidget.generated.h"

class UScaleBox;
class USizeBox;
class UCanvasPanel;
class UImage;
class UCanvasPanelSlot;

UCLASS()
class METATOOLKIT_API UMetaWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMetaWidget(const FObjectInitializer& ObjectInitializer);
	
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

	UPROPERTY(BlueprintReadWrite, Category="MetaWidget")
	FVector2D MousePosition = FVector2D(0, 0);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PlayerController")
	APlayerController* PlayerController;

private:
	UPROPERTY()
	UCanvasPanelSlot* MetaCursorCanvasPanelSlot;

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void MetaNativeOnViewportResized(FViewport* Viewport, unsigned int I);

public:
	UFUNCTION(BlueprintCallable, Category="MetaWidget")
	void SetSizeBoxSize(const int32 Width, const int32 Height);
};
