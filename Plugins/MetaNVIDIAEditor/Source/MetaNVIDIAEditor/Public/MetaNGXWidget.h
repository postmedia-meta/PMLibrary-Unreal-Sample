// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MNESaveGame.h"
#include "Blueprint/UserWidget.h"
#include "MetaNGXWidget.generated.h"

class UMetaNVIDIASettingsComponent;
class UButton;

UCLASS()
class METANVIDIAEDITOR_API UMetaNGXWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="NVIDIA")
	UButton* ApplyButton;

private:
	UPROPERTY()
	UMNESaveGame* MNESaveGame;

	UPROPERTY()
	UMetaNVIDIASettingsComponent* MetaNVIDIASettingsComponent;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UFUNCTION(BlueprintCallable, Category="NVIDIA")
	void ApplySettings();

	UFUNCTION(BlueprintCallable, Category="NVIDIA")
	void ShowWidget();

	UFUNCTION(BlueprintCallable, Category="NVIDIA")
	void HideWidget();
};
