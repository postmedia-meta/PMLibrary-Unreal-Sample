// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MetaMagicTrailWidget.generated.h"

class UMetaMagicTrailComponent;
class UMetaMagicTrailSaveGame;
class UScaleBox;
class USizeBox;
class UCanvasPanel;
class UImage;
class UButton;
class UCheckBox;
class UEditableText;
class UCanvasPanelSlot;
class UComboBoxString;

UCLASS()
class METAVFX_API UMetaMagicTrailWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UScaleBox* ScaleBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	USizeBox* SizeBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UCanvasPanel* CanvasPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UImage* MetaCursor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UEditableText* InitCreatePoolNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UEditableText* RateScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UEditableText* LifeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UEditableText* Scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UEditableText* SpriteRateScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UEditableText* SpriteScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UEditableText* EdgeThickness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UEditableText* EdgeIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UEditableText* ParticleActivationThresholdSec;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UEditableText* ResetTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UEditableText* EdgeColorR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UEditableText* EdgeColorG;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UEditableText* EdgeColorB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UEditableText* SpriteColorR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UEditableText* SpriteColorG;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UEditableText* SpriteColorB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UCheckBox* AutoColorCheckBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UCheckBox* UseMouseCheckBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UComboBoxString* MaskShapeComboBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UButton* ApplyButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UButton* CloseButton;
	
private:
	UPROPERTY(VisibleAnywhere, Category="SaveGame")
	UMetaMagicTrailSaveGame* MetaMagicTrailSaveGame;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	UMetaMagicTrailComponent* MetaMagicTrailComponent;

	UPROPERTY()
	UCanvasPanelSlot* MetaCursorCanvasPanelSlot = nullptr;

	UPROPERTY()
	FVector2D MousePosition = FVector2D(0, 0);

	UPROPERTY()
	FVector2D OriginViewportSize = FVector2D(0, 0);

	UPROPERTY()
	FVector2D OriginCursorSize = FVector2D(0, 0);

	UPROPERTY()
	EMouseCaptureMode OriginMouseCaptureMode;

	UPROPERTY()
	EMouseLockMode OriginMouseLockMode;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	UFUNCTION(BlueprintCallable, Category="MetaMagicTrailWidget")
	void InitVariable();
	
	UFUNCTION(BlueprintCallable, Category="MetaMagicTrailWidget")
	void ShowWidget();

	UFUNCTION(BlueprintCallable, Category="MetaMagicTrailWidget")
	void HideWidget();

	UFUNCTION(BlueprintCallable, Category="MetaMagicTrailWidget")
	void ApplySettings();

	UFUNCTION(BlueprintCallable, Category="MetaMagicTrailWidget")
	void SetSizeBoxSize(const int32 Width, const int32 Height);

	UFUNCTION(BlueprintCallable, Category="MetaMagicTrailWidget")
	void MetaCursorResizing();

private:
	UFUNCTION()
	void OnChangedInitCreatePoolNum(const FText& Text, ETextCommit::Type CommitMethod);
	
	UFUNCTION()
	void OnChangedRateScale(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnChangedLifeTime(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnChangedScale(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnChangedEdgeThickness(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnChangedIntensity(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnChangedParicleActivationThresholdSec(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnChangedResetTime(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnChangedEdgeColorR(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnChangedEdgeColorG(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnChangedEdgeColorB(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnChangedSpriteColorR(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnChangedSpriteColorG(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnChangedSpriteColorB(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnSelectionChangedMaskShape(FString SelectedItem, ESelectInfo::Type SelectionType);
};
