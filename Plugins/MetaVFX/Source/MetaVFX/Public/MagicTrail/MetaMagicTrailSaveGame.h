// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MetaMagicTrailManager.h"
#include "GameFramework/SaveGame.h"
#include "MetaMagicTrailSaveGame.generated.h"

enum class EMaskShape : uint8;

USTRUCT(Blueprintable)
struct FMetaMagicTrailData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FMetaMagicTrailData")
	int32 InitCreatePoolNum = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FMetaMagicTrailData")
	float RateScale = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FMetaMagicTrailData")
	float LifeTime = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FMetaMagicTrailData")
	float Scale = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FMetaMagicTrailData")
	float SpriteRateScale = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FMetaMagicTrailData")
	float SpriteScale = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FMetaMagicTrailData")
	float EdgeThickness = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FMetaMagicTrailData")
	float EdgeIntensity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FMetaMagicTrailData")
	FColor EdgeColor = FColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FMetaMagicTrailData")
	FColor SpriteColor = FColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FMetaMagicTrailData")
	bool bAutoColor = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FMetaMagicTrailData", meta=(ClampMin=0.1, UIMin = 0.1))
	float ParticleActivationThresholdSec = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FMetaMagicTrailData", meta=(ClampMin=1, UIMin = 1))
	float ResetTime = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FMetaMagicTrailData")
	bool bUseMouse = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FMetaMagicTrailData")
	bool bUseFile = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FMetaMagicTrailData")
	EMaskShape MaskShape = EMaskShape::Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FMetaMagicTrailData")
	FString FilePath = TEXT("");
};

UCLASS()
class METAVFX_API UMetaMagicTrailSaveGame : public USaveGame
{
	GENERATED_BODY()

	UMetaMagicTrailSaveGame();
	virtual ~UMetaMagicTrailSaveGame() override;

	static void LoadSaveGame();
	
	static UMetaMagicTrailSaveGame* MetaMagicTrailSaveGame;

public:
	UFUNCTION(BlueprintGetter, Category="SaveGame|MetaMagicTrailSaveGame", meta=(DisplayName="Get MetaMagicTrailSaveGame singleton instance"))
	static UMetaMagicTrailSaveGame* Get();

	UFUNCTION(BlueprintCallable, Category="SaveGame|MetaMagicTrailSaveGame")
	void SaveGame();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="SaveGame|MetaMagicTrailSaveGame|Settings")
	FMetaMagicTrailData MetaMagicTrailData;

	static FString SaveSlotName;
	static uint32 UserIndex;
};
