// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MNESaveGame.h"
#include "Components/ActorComponent.h"
#include "MetaNVIDIASettingsComponent.generated.h"

class UUserWidget;
class UMetaNGXWidget;

UCLASS(ClassGroup = "NVIDIA", meta=(BlueprintSpawnableComponent))
class METANVIDIAEDITOR_API UMetaNVIDIASettingsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMetaNVIDIASettingsComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Meta NVIDIA Settings")
	UMetaNGXWidget* MetaNGXWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Meta NVIDIA Settings")
	TSubclassOf<UUserWidget> MetaNGXWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Meta NVIDIA Settings")
	int32 ZOrder = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NVIDIA")
	FNVIDIASettings NVIDIASettings;
	
private:
	UPROPERTY()
	UMNESaveGame* MNESaveGame;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category="Meta NVIDIA Settings")
	void ShowNGXUI();
	
	UFUNCTION(BlueprintCallable, Category="Meta NVIDIA Settings")
	void HideNGXUI();

	UFUNCTION(BlueprintPure, Category="Meta NVIDIA Settings")
	bool IsShowWidget();
};
