// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Components/ActorComponent.h"
#include "MetaFluidArtActorComponent.generated.h"


class UNiagaraSystem;
class UNiagaraComponent;

UCLASS( ClassGroup="MetaVFX", meta=(BlueprintSpawnableComponent) )
class METAVFX_API UMetaFluidArtActorComponent : public UNiagaraComponent
{
	GENERATED_BODY()

	UPROPERTY()
	UNiagaraSystem* NiagaraSystem;

	UPROPERTY(VisibleAnywhere)
	TArray<UTexture2D*> Textures;
	
public:
	UMetaFluidArtActorComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaFluidArtActor")
	bool bAutoNoise;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaFluidArtActor")
	float SpriteScale = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaFluidArtActor")
	float NoiseForce = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaFluidArtActor")
	float NoiseFrequency = 20;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaFluidArtActor")
	int32 NumCellsMaxAxis = 50;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaFluidArtActor")
	int32 ParticlePerCell = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaFluidArtActor")
	int32 PressureIterations = 150;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaFluidArtActor")
	float TextureChangeDelay = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaFluidArtActor")
	FVector2D UVStep = FVector2D::Zero();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaFluidArtActor")
	TArray<FString> FilePaths;

private:
	int32 CurrentIndex = -1;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;

public:	
	UFUNCTION(BlueprintCallable, Category="MetaFluidArtActor")
	void NextTexture();
};
