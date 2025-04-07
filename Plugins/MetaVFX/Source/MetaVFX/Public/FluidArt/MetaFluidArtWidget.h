// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MetaFluidArtWidget.generated.h"

class UCheckBox;
class UMetaFluidArtManager;
class UMetaFluidArtActorComponent;
class UMetaFluidArtSaveGame;
class UComboBoxString;
class UScaleBox;
class USizeBox;
class UCanvasPanel;
class UImage;
class UEditableText;
class UTextBlock;
class UButton;
class UCanvasPanelSlot;

UCLASS()
class METAVFX_API UMetaFluidArtWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget")
	UScaleBox* ScaleBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget")
	USizeBox* SizeBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget")
	UCanvasPanel* CanvasPanel;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget")
	UImage* MetaCursor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget|Controller")
	UEditableText* InitCreatePoolNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget|Controller")
	UEditableText* InteractionScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget|Controller")
	UEditableText* ScaleIncreaseTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget|Controller")
	UEditableText* ScaleDecreaseTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget|Particle")
	UEditableText* NoiseForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget|Particle")
	UEditableText* NoiseFrequency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget|Particle")
	UEditableText* SpriteScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget|Particle")
	UEditableText* TextureChangeDelay;

	// Maximum number of cells per axis (Associated with the number of particles)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget|Particle")
	UEditableText* NumCellsMaxAxis;

	// Number of particles per cell (Associated with the number of particles)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget|Particle")
	UEditableText* ParticlePerCell;

	// Accuracy and Relationships in Fluid Simulation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget|Particle")
	UEditableText* PressureIterations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget|Particle")
	UEditableText* UVStepU;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget|Particle")
	UEditableText* UVStepV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget|Particle")
	UComboBoxString* PathsComboBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget|Particle")
	UCheckBox* AutoNoiseCheckBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaMagicTrailWidget")
	UButton* SelectedPathButton;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget")
	UButton* ClearPathsButton;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget")
	UButton* ApplyButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="MetaFluidArtWidget")
	UButton* CloseButton;

private:
	UPROPERTY(VisibleAnywhere, Category="SaveGame")
	UMetaFluidArtSaveGame* MetaFluidArtSaveGame;
	
	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	UMetaFluidArtManager* MetaFluidArtControlComponent;

	UPROPERTY()
	UCanvasPanelSlot* MetaCursorCanvasPanelSlot = nullptr;

	UPROPERTY()
	FVector2D MousePosition = FVector2D(0, 0);

	UPROPERTY()
	FVector2D OriginViewportSize = FVector2D(0, 0);

	UPROPERTY()
	FVector2D OriginCursorSize = FVector2D(0, 0);

	UPROPERTY()
	bool PrevHideCursorDuringCapture;

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
	void OnChangedTextureInteractionScale(const FText& Text, ETextCommit::Type Arg);
	
	UFUNCTION()
	void OnChangedTextureScaleIncreaseTime(const FText& Text, ETextCommit::Type Arg);

	UFUNCTION()
	void OnChangedTextureScaleDecreaseTime(const FText& Text, ETextCommit::Type Arg);
	
	UFUNCTION()
	void OnChangedNoiseForce(const FText& Text, ETextCommit::Type CommitMethod);
	
	UFUNCTION()
	void OnChangedNoiseFrequency(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnChangedSpriteScale(const FText& Text, ETextCommit::Type CommitMethod);
	
	UFUNCTION()
	void OnChangedTextureChangeDelay(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnChangedNumCellsMaxAxis(const FText& Text, ETextCommit::Type CommitMethod);
	
	UFUNCTION()
	void OnChangedParticlePerCell(const FText& Text, ETextCommit::Type CommitMethod);
	
	UFUNCTION()
	void OnChangedPressureIterations(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnChangedUVStepU(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnChangedUVStepV(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnOpenFileButton();
	
	UFUNCTION()
	void OnClearFileButton();

	UFUNCTION()
	void OnPathSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
};
