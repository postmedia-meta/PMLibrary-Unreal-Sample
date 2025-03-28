// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GenericPlatform/GenericWindow.h"
#include "MTKSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FGraphicsSettings
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="Graphics")
	FIntPoint Resolution = FIntPoint(1920, 1080);

	UPROPERTY(VisibleAnywhere, Category="Graphics")
	uint8 WindowMode = EWindowMode::Windowed;

	UPROPERTY(VisibleAnywhere, Category="Graphics")
	uint8 OverallQuality = 3;

	UPROPERTY(VisibleAnywhere, Category="Graphics")
	uint8 ViewDistanceQuality = 3;

	UPROPERTY(VisibleAnywhere, Category="Graphics")
	uint8 GlobalIlluminationQuality = 3;

	UPROPERTY(VisibleAnywhere, Category="Graphics")
	uint8 ShadowQuality = 3;

	UPROPERTY(VisibleAnywhere, Category="Graphics")
	uint8 PostProcessingQuality = 3;

	UPROPERTY(VisibleAnywhere, Category="Graphics")
	uint8 AntiAliasingQuality = 3;

	UPROPERTY(VisibleAnywhere, Category="Graphics")
	uint8 ReflectionQuality = 3;
	
	UPROPERTY(VisibleAnywhere, Category="Graphics")
	bool bEnabledScreenMessage = true;
};

UCLASS()
class METATOOLKIT_API UMTKSaveGame : public USaveGame
{
	GENERATED_BODY()

	UMTKSaveGame();
	virtual ~UMTKSaveGame() override;

	static void LoadSaveGame();
	
	static UMTKSaveGame* MTKSaveGame;

public:
	UFUNCTION(BlueprintGetter, Category="SaveGame|MetaToolkit", meta=(DisplayName="Get MetaToolkit singleton instance"))
	static UMTKSaveGame* Get();

	UFUNCTION(BlueprintCallable, Category="SaveGame|MetaToolkit")
	void SaveGame();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="SaveGame|MetaToolkit|Settings")
	FGraphicsSettings GraphicsSettings;

	static FString SaveSlotName;
	static uint32 UserIndex;
};