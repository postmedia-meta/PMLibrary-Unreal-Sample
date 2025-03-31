#pragma once

#include "CoreMinimal.h"
#include "Components//ActorComponent.h"
#include "MetaMagicTrailComponent.generated.h"

class UMemoryPoolObject;
class ANiagaraActor;
class UNiagaraSystem;
class UNiagaraComponent;

USTRUCT(Blueprintable)
struct FLiDARActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="LiDARActor")
	int32 ID;

	UPROPERTY(VisibleAnywhere, Category="LiDARActor")
	AActor* Actor;

	UPROPERTY(VisibleAnywhere, Category="LiDARActor")
	UNiagaraComponent* NiagaraComponent;
	
	UPROPERTY(VisibleAnywhere, Category="LiDARActor")
	FTimerHandle DelayHandle;
};

UCLASS(ClassGroup = "MetaVFX", meta = (BlueprintSpawnableComponent))
class METAVFX_API UMetaMagicTrailComponent : public UActorComponent
{
	GENERATED_BODY()

	UMetaMagicTrailComponent();
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MetaMagicTrail")
	UMemoryPoolObject* MemoryPoolObject;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MetaMagicTrail")
	TSubclassOf<ANiagaraActor> MagicTrailNiagaraActorClass;

	// Masking texture
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle")
	UTexture2D* Texture2D;

	// particle rate scale
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle")
	float RateScale = 1;

	// particle lifetime
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle")
	float LifeTime = 10;

	// particle scale
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle")
	float Scale = 1;

	// The smaller the value, the thicker it becomes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle")
	float EdgeThickness = 1;

	// The higher the value, the brighter the ends.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle")
	float EdgeIntensity = 1;

	// Edge color of the particle being masked
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle")
	FColor EdgeColor = FColor::White;

	// Scattered particle particles color
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle")
	FColor SpriteColor = FColor::White;

	// If true, the particle color will be affected by the texture color.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle")
	bool bAutoColor = true;

	// Input is detected and masking(Particle) begins after a delay.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail", meta=(ClampMin=0.1, UIMin = 0.1))
	float ParticleActivationThresholdSec = 0.1;

	// A cycle that deletes all particles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail", meta=(ClampMin=1, UIMin = 1))
	float ResetTime = 10;

	// Whether to use a mouse
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail")
	bool bUseMouse = true;

	// If false, change the mouse position directly to the world position without using ray.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail")
	bool bUseRay = true;

private:
	float CurrentTime = 0;

	bool bIsDragging = false;

	FIntPoint ViewportSize;
	
	UPROPERTY(VisibleAnywhere, Category="MetaMagicTrail")
	UMaterial* SpriteMaterial;

	UPROPERTY(VisibleAnywhere, Category="MetaMagicTrail")
	UMaterial* TrailMaterial;
	
	UPROPERTY(VisibleAnywhere, Category="MetaMagicTrail")
	UMaterialInstanceDynamic* SpriteMaterialInstanceDynamic;
	
	UPROPERTY(VisibleAnywhere, Category="MetaMagicTrail")
	UMaterialInstanceDynamic* TrailMaterialInstanceDynamic;
	
	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	AActor* MouseActor;

	UPROPERTY()
	UNiagaraComponent* MouseNiagaraComponent;

	FTimerHandle MouseDelayHandle;

	UPROPERTY()
	TArray<FLiDARActor> LiDARActors;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable)
	void NewMagicTrailsWithLiDAR(const TArray<int32> IDs, const TArray<FVector2D> Percentages);

	UFUNCTION(BlueprintCallable)
	void UpdateMagicTrailsWithLiDAR(const TArray<int32> IDs, const TArray<FVector2D> Percentages);

	UFUNCTION(BlueprintCallable)
	void RemoveMagicTrailsWithLiDAR(const TArray<int32> IDs);

	UFUNCTION(BlueprintCallable)
	void DeallocateAllMagicTrails();

private:
	void OnViewportResized(FViewport* Viewport, unsigned int I);
	
	UFUNCTION()
	void OnLeftMouseDown();

	UFUNCTION()
	void OnLeftMouseUp();

	UFUNCTION()
	void LeftMouseDrag();

	UFUNCTION()
	void SetMouseActorLocation();

	UFUNCTION()
	void SetActorLocationToScreenPosition(FLiDARActor& Actor, const float X, const float Y);
};