// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MTKWidget.h"
#include "MTKGraphicsSettingsWidget.generated.h"

class UMTKArrowSwitchWidget;
class UCanvasPanelSlot;
class UTextBlock;
class UButton;
class UComboBoxString;
class UEditableText;
class UMTKSaveGame;

UCLASS(meta=(DisableNativeTick))
class METATOOLKIT_API UMTKGraphicsSettingsWidget : public UMTKWidget
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
	UMTKArrowSwitchWidget* OverallQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UMTKArrowSwitchWidget* ViewDistanceQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UMTKArrowSwitchWidget* GlobalIlluminationQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UMTKArrowSwitchWidget* ShadowQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UMTKArrowSwitchWidget* PostProcessingQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UMTKArrowSwitchWidget* AntiAliasingQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UMTKArrowSwitchWidget* ReflectionQuality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UButton* ApplyButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UButton* CloseButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UButton* ScreenMessageButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="GraphicSettingWidget")
	UTextBlock* ScreenMessageStateTextBlock;

private:
	TMap<uint8, EWindowMode::Type> WindowModeMap = {
		{0, EWindowMode::Type::Fullscreen},
		{1, EWindowMode::Type::WindowedFullscreen},
		{2, EWindowMode::Type::Windowed}
	};

	FTimerHandle InitTimerHandle;

	UPROPERTY(VisibleAnywhere, Category="SaveGame")
	UMTKSaveGame* MTKSaveGame;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void MetaNativeOnViewportResized(FViewport* Viewport, unsigned int I) override;

private:
	UFUNCTION()
	void Setup();
	
public:
	UFUNCTION(BlueprintCallable, Category="GraphicSettingWidget")
	void InitVariable();

	UFUNCTION(BlueprintCallable, Category="GraphicSettingWidget")
	void ChangeResolutionX(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION(BlueprintCallable, Category="GraphicSettingWidget")
	void ChangeResolutionY(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION(BlueprintCallable, Category="GraphicSettingWidget")
	void ChangeOverallQuality(const int32 Quality);
	
	UFUNCTION(BlueprintCallable, Category="GraphicSettingWidget")
	void ApplySettings();

	UFUNCTION(BlueprintCallable, Category="GraphicSettingWidget")
	void ApplyResolutionSettings();

	UFUNCTION(BlueprintCallable, Category="GraphicSettingWidget")
	void ApplyGraphicsSettings();

	UFUNCTION(BlueprintCallable, Category="GraphicSettingWidget")
	void ShowWidget();

	UFUNCTION(BlueprintCallable, Category="GraphicSettingWidget")
	void HideWidget();

	UFUNCTION(BlueprintCallable, Category="GraphicSettingWidget")
	void ToggleScreenMessage();
};
