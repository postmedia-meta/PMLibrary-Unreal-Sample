// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Components/ActorComponent.h"
#include "MetaFluidArtActorComponent.generated.h"


class UNiagaraSystem;
class UNiagaraComponent;
class USplineComponent;

UCLASS( ClassGroup="MetaVFX", meta=(BlueprintSpawnableComponent) )
class METAVFX_API UMetaFluidArtActorComponent : public UNiagaraComponent
{
	GENERATED_BODY()

	UPROPERTY()
	UNiagaraSystem* NiagaraSystem;

	UPROPERTY(VisibleAnywhere, Category="MetaFluidArtActor")
	TArray<UTexture2D*> Textures;

	UPROPERTY()
	UStaticMesh* StaticMesh;

	UPROPERTY()
	TArray<AStaticMeshActor*> NoiseColliders;
	
public:
	UMetaFluidArtActorComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise")
	bool bColliderVisible = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise")
	float Duration = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise")
	int32 ColliderNum = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise")
	FVector NoiseColliderScale = FVector::One();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise")
	FVector StartLocationOffset = FVector(0, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise")
	FVector NoiseRange = FVector(100, 640, 450);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise")
	float MinLength = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Noise")
	float MaxLength = 100;

private:
	bool bAutoNoise;

	float SpriteScale = 1;

	float NoiseForce = 30;

	float NoiseFrequency = 20;
	
	int32 NumCellsMaxAxis = 50;
	
	int32 ParticlePerCell = 5;

	int32 PressureIterations = 150;

	float TextureChangeDelay = 1;

	FVector2D UVStep = FVector2D::Zero();
	
	TArray<FString> FilePaths;
	
	int32 CurrentIndex = -1;
	UPROPERTY(VisibleAnywhere)
	float TotalTime = 0;

	UPROPERTY(VisibleAnywhere)
	float ElapsedTime = 0;
	
	FTimerHandle TextureChangeHandle;

	FTimerHandle NoiseHandle;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	UFUNCTION(BlueprintCallable, Category="MetaFluidArtActor")
	void NextTexture();

	UFUNCTION(BlueprintCallable, Category="MetaFluidArtActor")
	void UpdateNoiseMovement();

private:
	void UpdateSpline(USplineComponent* SplineComponent);

	void UpdateNoiseCollider();
};
