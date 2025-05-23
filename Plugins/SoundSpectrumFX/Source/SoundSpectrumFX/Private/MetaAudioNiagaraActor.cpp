// Fill out your copyright notice in the Description page of Project Settings.


#include "MetaAudioNiagaraActor.h"

#include "MetaAudioCaptureComponent.h"
#include "NiagaraComponent.h"
#include "AudioMixerBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

#if WITH_EDITOR
void AMetaAudioNiagaraActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.GetPropertyName().IsEqual(TEXT("Frequencies")))
	{
		const int32 Diff = Frequencies.Num() - Colors.Num();
		if (Diff > 0)
		{
			for (int i = 0; i < Diff; ++i)
			{
				Colors.Add(BaseColor);
			}
		}
		else if (Diff < 0)
		{
			for (int i = 0; i > Diff; --i)
			{
				Colors.Pop();
			}
		}
	}

	if (PropertyChangedEvent.GetPropertyName().IsEqual(TEXT("Colors")))
	{
		const int32 Diff = Colors.Num() - Frequencies.Num();
		if (Diff > 0)
		{
			for (int i = 0; i < Diff; ++i)
			{
				float Freq = FMath::Min(Frequencies[Frequencies.Num() - 1] * 2, 20000);
				Frequencies.Add(Freq);
			}
		}
		else if (Diff < 0)
		{
			for (int i = 0; i > Diff; --i)
			{
				Frequencies.Pop();
			}
		}
	}
}
#endif

AMetaAudioNiagaraActor::AMetaAudioNiagaraActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	MetaAudioCaptureComponent = CreateDefaultSubobject<UMetaAudioCaptureComponent>(TEXT("Meta Audio Capture"));
	MetaAudioCaptureComponent->SetupAttachment(GetRootComponent());
}

void AMetaAudioNiagaraActor::BeginPlay()
{
	Super::BeginPlay();

	CurrentColor = BaseColor;
	GetNiagaraComponent()->SetVariableLinearColor(TEXT("Color"), CurrentColor);
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), GetClass(), SameClassActors);
	
	if (bPlayOnStart)
	{
		MetaAudioCaptureComponent->Start();
		MetaAudioCaptureComponent->StartAnalyzingMicrophone(FrequencyBandFeatureType);
		UAudioMixerBlueprintLibrary::StartAnalyzingOutput(GetWorld());
	}
}

void AMetaAudioNiagaraActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	OnMagnitudesForFrequencies.Clear();
	OnMaxMagnitude.Clear();

	MetaAudioCaptureComponent->Stop();
	MetaAudioCaptureComponent->StopAnalyzingMicrophone();
	UAudioMixerBlueprintLibrary::StopAnalyzingOutput(GetWorld());
}

void AMetaAudioNiagaraActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (!Frequencies.IsEmpty())
	{
		TArray<float> MicMagnitudes;
		TArray<float> MixerMagnitudes;
		TArray<float> MultipliedMagnitudes;

		int32 MaxMagnitudeIndex = 0;
		float MaxMagnitude = 0;
		float Amplitude = 0;
		float AudioFrequency = 0;
		
		// 오디오, 마이크 주파수별 크기 분석
		// GetMagnitudeForFrequencies을 같은 프레임에서 반복해서 호출시 발생할 수 있는 문제를 최소화히기 위해 프레임을 지연시키며 분석
		if (Analyze != EAnalyze::MicOnly)
		{
			for (int i = 0; i < SameClassActors.Num(); ++i)
			{
				if (SameClassActors[i] == this)
				{
					if (GFrameCounter % SameClassActors.Num() == i)
					{
						UAudioMixerBlueprintLibrary::GetMagnitudeForFrequencies(GetWorld(), Frequencies, MixerMagnitudes);
					}
					else
					{
						return;
					}
				}
			}	
		}
		MetaAudioCaptureComponent->GetMagnitudeForFrequencies(Frequencies, MicMagnitudes);

		if (MixerMagnitudes.Num() != MicMagnitudes.Num() && Frequencies.Num() != MicMagnitudes.Num()) return;

		for (int32 i = 0; i < Frequencies.Num(); ++i)
		{
			// 마이크와 믹서 중 더 큰 크기로 초기화
			float Magnitude = 0;
			
			switch (Analyze)
			{
			case EAnalyze::MicOnly:
				Magnitude = MicMagnitudes[i] * MicMultiplier;
				break;
			case EAnalyze::AudioOnly:
				Magnitude = MixerMagnitudes.IsEmpty() ? 0 : MixerMagnitudes[i] * AudioMultiplier;
				break;
			case EAnalyze::Both:
				Magnitude = MixerMagnitudes.IsEmpty() ? MicMagnitudes[i] * MicMultiplier :  FMath::Max(MicMagnitudes[i] * MicMultiplier, MixerMagnitudes[i] * AudioMultiplier);
			}
			
			// 가장 큰 크기의 주파수 찾기
			if (MaxMagnitude < Magnitude)
			{
				MaxMagnitudeIndex = i;
				MaxMagnitude = Magnitude;
			}
			
			// 주파수 크기 보정
			MultipliedMagnitudes.Add(FMath::Min(Magnitude, 1));
		}

		if (MinMagnitudes < MultipliedMagnitudes[MaxMagnitudeIndex])
		{
			TargetColor = Colors[MaxMagnitudeIndex];
			Amplitude = MultipliedMagnitudes[MaxMagnitudeIndex];
			AudioFrequency = Frequencies[MaxMagnitudeIndex];

			UpdateCurrentColor();
			
			// 나이아가라에 주파수와 크기 전달
			GetNiagaraComponent()->SetVariableFloat(TEXT("Audio Frequency"), AudioFrequency);
			GetNiagaraComponent()->SetVariableFloat(TEXT("Amplitude"), Amplitude);
			GetNiagaraComponent()->SetVariableLinearColor(TEXT("Color"), CurrentColor);
    
			// 델리게이트 이벤트 호출 (마이크 주파수별 크기를 전달)
			if (OnMagnitudesForFrequencies.IsBound())
			{
				OnMagnitudesForFrequencies.Broadcast(MultipliedMagnitudes);
			}
    
			if (OnMaxMagnitude.IsBound())
			{
				OnMaxMagnitude.Broadcast(AudioFrequency, Amplitude);
			}
		}
		else
		{
			ResetCurrentColor();
		}

		GetNiagaraComponent()->SetVariableFloat(TEXT("Audio Frequency"), AudioFrequency);
		GetNiagaraComponent()->SetVariableFloat(TEXT("Amplitude"), Amplitude);
		GetNiagaraComponent()->SetVariableLinearColor(TEXT("Color"), CurrentColor);
	}
}

void AMetaAudioNiagaraActor::StartInteraction(const EFrequencyBandFeatureType Type)
{
	FrequencyBandFeatureType = Type;
	
	MetaAudioCaptureComponent->Start();
	MetaAudioCaptureComponent->StartAnalyzingMicrophone(Type);
	UAudioMixerBlueprintLibrary::StartAnalyzingOutput(GetWorld());
}

void AMetaAudioNiagaraActor::StopInteraction()
{
	MetaAudioCaptureComponent->Stop();
	MetaAudioCaptureComponent->StopAnalyzingMicrophone();
	UAudioMixerBlueprintLibrary::StopAnalyzingOutput(GetWorld());
}

void AMetaAudioNiagaraActor::UpdateCurrentColor()
{
	if (CurrentColor == TargetColor) return;
	
	CurrentColor = FLinearColor::LerpUsingHSV(CurrentColor, TargetColor, ColorChangeSpeed);

	if (OnColorChanged.IsBound())
	{
		OnColorChanged.Broadcast(CurrentColor);
	}
}

void AMetaAudioNiagaraActor::ResetCurrentColor()
{
	if (TargetColor != BaseColor) TargetColor = BaseColor;
	if (CurrentColor == TargetColor) return;
	
	CurrentColor = FLinearColor::LerpUsingHSV(CurrentColor, TargetColor, ColorResetSpeed);

	if (OnColorChanged.IsBound())
	{
		OnColorChanged.Broadcast(CurrentColor);
	}
}
