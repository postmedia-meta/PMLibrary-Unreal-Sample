// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MetaFluidArtSaveGame.generated.h"

USTRUCT(Blueprintable)
struct FNiagaraData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NiagaraData")
	bool bAutoNoise = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NiagaraData")
	float NoiseForce = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NiagaraData")
	float NoiseFrequency = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NiagaraData")
	float SpriteScale = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NiagaraData")
	int32 NumCellsMaxAxis = 50;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NiagaraData")
	int32 ParticlePerCell = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NiagaraData")
	int32 PressureIterations = 150;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NiagaraData")
	FVector2D UVStep = FVector2D::Zero();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NiagaraData")
	float TextureChangeDelay = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NiagaraData")
	TArray<FString> FilePaths;
};

USTRUCT(Blueprintable)
struct FControllerData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ControllerData")
	int32 InitCreatePoolNum = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ControllerData")
	float InteractionScale = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ControllerData")
	float ScaleIncreaseTime = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ControllerData")
	float ScaleDecreaseTime = 1;
};


UCLASS()
class METAVFX_API UMetaFluidArtSaveGame : public USaveGame
{
	GENERATED_BODY()

	UMetaFluidArtSaveGame();
	virtual ~UMetaFluidArtSaveGame() override;

	static void LoadSaveGame();
	
	static UMetaFluidArtSaveGame* MetaFluidArtSaveGame;

public:
	UFUNCTION(BlueprintGetter, Category="SaveGame|MetaFluidArtSaveGame", meta=(DisplayName="Get MetaFluidArtSaveGame singleton instance"))
	static UMetaFluidArtSaveGame* Get();

	UFUNCTION(BlueprintCallable, Category="SaveGame|MetaFluidArtSaveGame")
	void SaveGame();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="SaveGame|MetaFluidArtSaveGame|Settings")
	FNiagaraData NiagaraData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="SaveGame|MetaFluidArtSaveGame|Settings")
	FControllerData ControllerData;

	static FString SaveSlotName;
	static uint32 UserIndex;
};
