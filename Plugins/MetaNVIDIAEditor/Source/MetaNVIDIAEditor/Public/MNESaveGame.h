// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GenericPlatform/GenericWindow.h"
#include "MNESaveGame.generated.h"

USTRUCT(BlueprintType)
struct FNVIDIASettings
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="NVIDIA")
	uint8 UpscalingMode = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="NVIDIA")
	uint8 SuperResolutionMode = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="NVIDIA")
	uint8 FrameGenerationMode = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="NVIDIA")
	uint8 ReflexMode = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="NVIDIA")
	uint8 DeepDVCMode = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="NVIDIA")
	uint8 NISMode = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="NVIDIA")
	float SRSharpness = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="NVIDIA")
	float NISSharpness = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="NVIDIA")
	float DeepDVCIntensity = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="NVIDIA")
	float DeepDVCSaturationBoost = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="NVIDIA")
	bool bUseHardwareRayTracing = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="NVIDIA")
	bool bEnableRayReconstruction = false;
};

UCLASS()
class METANVIDIAEDITOR_API UMNESaveGame : public USaveGame
{
	GENERATED_BODY()

	UMNESaveGame();
	virtual ~UMNESaveGame() override;

	static void LoadSaveGame();
	
	static UMNESaveGame* MNESaveGame;

public:
	UFUNCTION(BlueprintGetter, Category="SaveGame|NVIDIA", meta=(DisplayName="Get NVIDIA SaveGame singleton instance"))
	static UMNESaveGame* Get();

	UFUNCTION(BlueprintCallable, Category="SaveGame|NVIDIA")
	void SaveGame();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="SaveGame|NVIDIA|Settings")
	FNVIDIASettings NVIDIASettings;

	static FString SaveSlotName;
	static uint32 UserIndex;
};