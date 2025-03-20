// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MetaGraphicsSettingsWidget.generated.h"

class UArrowSwitchWidget;
class UCanvasPanelSlot;
class UImage;
class UTextBlock;
class UButton;
class UComboBoxString;
class UEditableText;
class UScaleBox;
class USizeBox;
class UCanvasPanel;
class UMetaToolkitSaveGame;

UCLASS()
class METATOOLKIT_API UMetaGraphicsSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UTextBlock* ViewportSizeTextBlock;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UEditableText* ResolutionX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UEditableText* ResolutionY;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UComboBoxString* ModeComboBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UArrowSwitchWidget* OverallQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UArrowSwitchWidget* ViewDistanceQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UArrowSwitchWidget* GlobalIlluminationQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UArrowSwitchWidget* ShadowQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UArrowSwitchWidget* PostProcessingQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UArrowSwitchWidget* AntiAliasingQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UArrowSwitchWidget* ReflectionQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UButton* ApplyButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UButton* CloseButton;

private:
	FIntPoint ViewportSize = FIntPoint(0, 0);

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	UGameUserSettings* GameUserSettings;

	UPROPERTY(VisibleAnywhere)
	UMetaToolkitSaveGame* MetaToolkitSaveGame;

	TMap<uint8, EWindowMode::Type> WindowModeMap = {
		{0, EWindowMode::Type::Fullscreen},
		{1, EWindowMode::Type::WindowedFullscreen},
		{2, EWindowMode::Type::Windowed}
	};


protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	void InitVariable();

	UFUNCTION(BlueprintCallable)
	void ChangeResolutionX(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION(BlueprintCallable)
	void ChangeResolutionY(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION(BlueprintCallable)
	void ChangeOverallQuality(const int32 Quality);

	UFUNCTION(BlueprintCallable)
	void ShowWidget();

	UFUNCTION(BlueprintCallable)
	void HideWidget();
	
	UFUNCTION(BlueprintCallable)
	void ApplySettings();

	UFUNCTION(BlueprintCallable)
	void ApplyResolutionSettings();

	UFUNCTION(BlueprintCallable)
	void ApplyGraphicsSettings();
};
