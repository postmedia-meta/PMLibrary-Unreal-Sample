// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MetaToolkitSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FGraphicsSettings
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FIntPoint Resolution = FIntPoint(1920, 1080);

	UPROPERTY(VisibleAnywhere)
	uint8 WindowMode = EWindowMode::Windowed;

	UPROPERTY(VisibleAnywhere)
	uint8 OverallQuality = 3;

	UPROPERTY(VisibleAnywhere)
	uint8 ViewDistanceQuality = 3;

	UPROPERTY(VisibleAnywhere)
	uint8 GlobalIlluminationQuality = 3;

	UPROPERTY(VisibleAnywhere)
	uint8 ShadowQuality = 3;

	UPROPERTY(VisibleAnywhere)
	uint8 PostProcessingQuality = 3;

	UPROPERTY(VisibleAnywhere)
	uint8 AntiAliasingQuality = 3;

	UPROPERTY(VisibleAnywhere)
	uint8 ReflectionQuality = 3;
};

UCLASS()
class METATOOLKIT_API UMetaToolkitSaveGame : public USaveGame
{
	GENERATED_BODY()

	UMetaToolkitSaveGame();
	virtual ~UMetaToolkitSaveGame() override;

	static void LoadSaveGame();
	
	static UMetaToolkitSaveGame* MetaToolkitSaveGame;

public:
	UFUNCTION(BlueprintGetter, Category="SaveGame|MetaToolkit", meta=(DisplayName="Get MetaToolkit singleton instance"))
	static UMetaToolkitSaveGame* Get();

	UFUNCTION(BlueprintCallable, Category="SaveGame|MetaToolkit")
	void SaveGame();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="SaveGame|MetaToolkit|Settings")
	FGraphicsSettings GraphicsSettings;

	static FString SaveSlotName;
	static uint32 UserIndex;
};