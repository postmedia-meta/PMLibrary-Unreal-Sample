// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MetaAudioCaptureComponent.h"
#include "NiagaraActor.h"
#include "MetaAudioNiagaraActor.generated.h"

enum class EFrequencyBandFeatureType : uint8;
class UMetaAudioCaptureComponent;

UENUM(BlueprintType)
enum class EAnalyze : uint8
{
	MicOnly,
	AudioOnly,
	Both
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMicMagnitudesForFrequencies, const TArray<float>&, Magnitudes);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMicMaxMagnitude, float, Freqeuncy, float, Magnitude);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnColorChanged, const FLinearColor&, Color);

UCLASS()
class SOUNDSPECTRUMFX_API AMetaAudioNiagaraActor : public ANiagaraActor
{
	GENERATED_BODY()

	AMetaAudioNiagaraActor(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(VisibleAnywhere, Category="Component")
	UMetaAudioCaptureComponent* MetaAudioCaptureComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting")
	FLinearColor BaseColor = FLinearColor(0,0,0, 1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting")
	TArray<float> Frequencies = {
		100, 500, 1000, 5000, 10000, 20000
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting")
	TArray<FLinearColor> Colors = {
		FColor::FromHex(TEXT("FF0032FF")),
		FColor::FromHex(TEXT("FF8800FF")),
		FColor::FromHex(TEXT("FEFF00FF")),
		FColor::FromHex(TEXT("84FF00FF")),
		FColor::FromHex(TEXT("00FFB8FF")),
		FColor::FromHex(TEXT("0090FFFF"))
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting")
	EAnalyze Analyze = EAnalyze::MicOnly;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting")
	bool bPlayOnStart = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting")
	EFrequencyBandFeatureType FrequencyBandFeatureType = EFrequencyBandFeatureType::Peak;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting", meta=(ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f))
	float MinMagnitudes = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting")
	float MicMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting")
	float AudioMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting", meta=(ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f))
	float ColorChangeSpeed = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting", meta=(ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f))
	float ColorResetSpeed = 0.2f;

	UPROPERTY(BlueprintAssignable, Category="Event|Audio")
	FOnMicMagnitudesForFrequencies OnMagnitudesForFrequencies;
	
	UPROPERTY(BlueprintAssignable, Category="Event|Audio")
	FOnMicMaxMagnitude OnMaxMagnitude;

	UPROPERTY(BlueprintAssignable, Category="Event|Audio")
	FOnColorChanged OnColorChanged;

private:
	FLinearColor CurrentColor;
	FLinearColor TargetColor;

	UPROPERTY()
	TArray<AActor*> SameClassActors;
	
protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

public:
	UFUNCTION(BlueprintCallable, Category="Audio|Meta")
	void StartInteraction(const EFrequencyBandFeatureType Type);
	
	UFUNCTION(BlueprintCallable, Category="Audio|Meta")
	void StopInteraction();
	
private:
	void UpdateCurrentColor();
	void ResetCurrentColor();
};
