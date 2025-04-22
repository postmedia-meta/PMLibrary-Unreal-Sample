#pragma once

#include "CoreMinimal.h"
#include "Components//ActorComponent.h"
#include "MetaMagicTrailManager.generated.h"

class UMetaMagicTrailWidget;
class UMemoryPoolObject;
class ANiagaraActor;
class UNiagaraSystem;
class UNiagaraComponent;
class UUserWidget;
class UMediaPlayer;
class UMediaTexture;
class UFileMediaSource;
class UMaterial;
class UMaterialInstanceDynamic;
class UTexture;
class FViewport;

UENUM(Blueprintable)
enum class EMaskShape : uint8
{
	Default,
	Circle,
	Square,
	Circle_Translucent,
	Square_Translucent
};

USTRUCT(Blueprintable)
struct FLiDARActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="LiDARActor")
	AActor* Actor;

	UPROPERTY(VisibleAnywhere, Category="LiDARActor")
	UNiagaraComponent* NiagaraComponent;
	
	UPROPERTY(VisibleAnywhere, Category="LiDARActor")
	FTimerHandle DelayHandle;
};

UCLASS(ClassGroup = "MetaVFX", meta = (BlueprintSpawnableComponent))
class METAVFX_API UMetaMagicTrailManager : public UActorComponent
{
	GENERATED_BODY()

	UMetaMagicTrailManager();

	UPROPERTY()
	TSubclassOf<ANiagaraActor> MagicTrailNiagaraActorClass;

	UPROPERTY()
	TSubclassOf<UUserWidget> MagicTrailWidgetClass;

	UPROPERTY()
	UMetaMagicTrailWidget* MagicTrailWidget;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	UMaterial* DefaultSpriteMaterial;

	UPROPERTY()
	UMaterial* DefaultTrailMaterial;

	UPROPERTY()
	UMaterial* CircleTrailMaterial;

	UPROPERTY()
	UMaterial* SquareTrailMaterial;

	UPROPERTY()
	UMaterial* CircleTranslucentTrailMaterial;

	UPROPERTY()
	UMaterial* SquareTranslucentTrailMaterial;
	
	UPROPERTY()
	UMaterialInstanceDynamic* SpriteMaterialInstanceDynamic;
	
	UPROPERTY()
	UMaterialInstanceDynamic* TrailMaterialInstanceDynamic;

	UPROPERTY()
	UMediaPlayer* MediaPlayer;

	UPROPERTY()
	UMediaTexture* MediaTexture;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MetaMagicTrail")
	UMemoryPoolObject* MemoryPoolObject;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle")
	EMaskShape MaskShape = EMaskShape::Default;

	// Masking texture from file
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle")
	bool bUseFile = false;

	// Supported jpg/jpeg/png/bmp/tga/psd/exr
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle", meta=(EditCondition="bUseFile == true", EditConditionHides), AssetRegistrySearchable)
	FFilePath AssetPath;

	// Masking texture from file
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle", meta=(EditCondition="bUseFile == false", EditConditionHides), AssetRegistrySearchable)
	bool bVideo = false;
	
	// Masking texture
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle", meta=(EditCondition="bUseFile == false && bVideo == false", EditConditionHides))
	UTexture2D* Texture2D;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle", meta=(EditCondition="bUseFile == false && bVideo == true", EditConditionHides))
	UFileMediaSource* FileMediaSource;

	// particle rate scale
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle")
	float RateScale = 1;

	// particle lifetime
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle")
	float LifeTime = 10;

	// particle scale
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle")
	float Scale = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle")
	float SpriteRateScale = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail|Particle")
	float SpriteScale = 1;

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

	// The number of memory pool objects created when the game starts.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail", meta=(ClampMin=1, UIMin = 1))
	int32 InitCreatePoolNum = 10;

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

	// If false, change the mouse position directly to the world position without using ray.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaMagicTrail")
	bool bUseEditUI = true;

private:
	float CurrentTime = 0;

	bool bIsDragging = false;

	FIntPoint ViewportSize;

	UPROPERTY()
	TMap<int32, FLiDARActor> LiDARActors;

	UPROPERTY()
	AActor* MouseActor;

	UPROPERTY()
	UNiagaraComponent* MouseNiagaraComponent;

	FTimerHandle MouseDelayHandle;

	UPROPERTY()
	UTexture* TextureFromFile;

	UPROPERTY()
	UTexture* TextureFromAsset;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable, Category="MetaMagicTrail")
	void NewMagicTrailWithLiDAR(const int32 ID, const FVector2D Percentage);

	UFUNCTION(BlueprintCallable, Category="MetaMagicTrail")
	void UpdateMagicTrailWithLiDAR(const int32 ID, const FVector2D Percentage);

	UFUNCTION(BlueprintCallable, Category="MetaMagicTrail")
	void RemoveMagicTrailWithLiDAR(const int32 ID);

	UFUNCTION(BlueprintCallable, Category="MetaMagicTrail")
	void DeallocateAllMagicTrails();

	UFUNCTION(BlueprintCallable, Category="MetaMagicTrail")
	void ShowWidget();

	UFUNCTION(BlueprintCallable, Category="MetaMagicTrail")
	void HideWidget();

	UFUNCTION(BlueprintCallable, Category="MetaMagicTrail")
	void ChangeMaskShape(const EMaskShape Shape);

	UFUNCTION(BlueprintCallable, Category="MetaMagicTrail")
	bool SetFilePath(const FString& NewFilePath);

	UFUNCTION(BlueprintCallable, Category="MetaMagicTrail")
	void ClearFilePath();

private:
	void OnViewportResized(FViewport* Viewport, unsigned int I);
	
	UFUNCTION()
	void OnLeftMouseDown();

	UFUNCTION()
	void OnLeftMouseUp();

	UFUNCTION()
	void LeftMouseDrag();

	UFUNCTION()
	void InitNiagaraComponent(UNiagaraComponent* NiagaraComponent);

	UFUNCTION()
	void SetMouseActorLocation();

	UFUNCTION()
	void SetLiDARActorLocationFromScreenPercentage(const int32 ID, const float X, const float Y);

public:
	UFUNCTION(BlueprintGetter, Category="MetaMagicTrail")
	bool IsShowWidget();
};