// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AudioCaptureComponent.h"
#include "MetaAudioCaptureComponent.generated.h"

// ========================================================================
// UMetaAudioCaptureComponent
// Synth component class which implements USynthComponent
// This is a simple hello-world type example which generates a sine-wave
// tone using a DSP oscillator class and implements a single function to set
// the frequency. To enable example:
// 1. Ensure "SignalProcessing" is added to project's .Build.cs in PrivateDependencyModuleNames
// 2. Enable macro below that includes code utilizing SignalProcessing Oscilator
// ========================================================================

namespace Audio
{
	enum class EFFTScaling : uint8;
	class IFFTAlgorithm;
}

UENUM(BlueprintType)
enum class EFrequencyBandFeatureType : uint8
{
	Peak   UMETA(DisplayName = "Peak (Max) per Band"),
	Mean   UMETA(DisplayName = "Mean (Average) per Band"),
	Nearest UMETA(DisplayName = "Nearest FFT Bin"),
};

UCLASS(ClassGroup = MetaVFX, meta = (BlueprintSpawnableComponent))
class SOUNDSPECTRUMFX_API UMetaAudioCaptureComponent : public UAudioCaptureComponent
{
	GENERATED_BODY()

	bool bAnalyzingMicrophone = false;

	int32 FFTSize = 0;
	
	Audio::FAlignedFloatBuffer MagnitudeSpectrum;

public:
	UFUNCTION(BlueprintCallable, Category="Audio|Meta")
	void StartAnalyzingMicrophone(const EFrequencyBandFeatureType Type);

	UFUNCTION(BlueprintCallable, Category="Audio|Meta")
	void StopAnalyzingMicrophone();

	UFUNCTION(BlueprintCallable, Category="Audio|Meta")
	void GetMagnitudeForFrequencies(const TArray<float>& InFrequencies, TArray<float>& Magnitude);

private:
	EFrequencyBandFeatureType FrequencyBandFeature = EFrequencyBandFeatureType::Peak;

	FCriticalSection CriticalSection;
	
private:
	// Called when synth is created
	virtual bool Init(int32& SampleRate) override;

	// Called to generate more audio
	virtual int32 OnGenerateAudio(float* OutAudio, int32 NumSamples) override;
	
	void AnalyzeMicrophonePCM(const Audio::FAlignedFloatBuffer& PCMData, int32 NumSamples);
};