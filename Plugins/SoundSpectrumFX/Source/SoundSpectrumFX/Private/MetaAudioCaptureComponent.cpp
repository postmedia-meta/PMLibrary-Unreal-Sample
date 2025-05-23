// Fill out your copyright notice in the Description page of Project Settings.


#include "MetaAudioCaptureComponent.h"

#include "AudioDevice.h"
#include "DSP/AudioFFT.h"
#include "DSP/FFTAlgorithm.h"

bool UMetaAudioCaptureComponent::Init(int32& SampleRate)
{
	NumChannels = 1;

	return true;
}

int32 UMetaAudioCaptureComponent::OnGenerateAudio(float* OutAudio, int32 NumSamples)
{
	const int32 OutputSamplesGenerated = Super::OnGenerateAudio(OutAudio, NumSamples);

	if (bAnalyzingMicrophone)
	{
		Audio::FAlignedFloatBuffer PcmBuffer;
		PcmBuffer.AddUninitialized(NumSamples);
		FMemory::Memcpy(PcmBuffer.GetData(), OutAudio, NumSamples * sizeof(float));
	
		AnalyzeMicrophonePCM(PcmBuffer, NumSamples);
	}
	else
	{
		MagnitudeSpectrum.Empty();
	}
	
	return OutputSamplesGenerated;
}

void UMetaAudioCaptureComponent::AnalyzeMicrophonePCM(const Audio::FAlignedFloatBuffer& PCMData, int32 NumSamples)
{
	using namespace Audio;

    // 1. FFT 크기 설정 (2의 거듭제곱이어야 함, 예: 1024, 2048, 4096)
	FFTSize = FMath::Pow(2.0f, CeilLog2(NumSamples));

	// 2. 입력 데이터를 준비합니다 (모노로 변환)
    FAlignedFloatBuffer InputBuffer = PCMData;
    
    // 남은 공간을 0으로 채웁니다
    if (NumSamples < FFTSize) {
        FMemory::Memset(InputBuffer.GetData() + NumSamples, 0, (FFTSize - NumSamples) * sizeof(float));
    }
    
    // 3. 윈도우 함수 적용 (선택 사항이지만 스펙트럼 누출을 줄이기 위해 권장)
    FWindow Window(Audio::EWindowType::Hann, FFTSize, 1, false);
    Window.ApplyToBuffer(InputBuffer.GetData());
    
    // 4. FFT 알고리즘 설정
    FFFTSettings FFTSettings;
    FFTSettings.Log2Size = CeilLog2(FFTSize);
    FFTSettings.bArrays128BitAligned = true;
    FFTSettings.bEnableHardwareAcceleration = true;
    
    TUniquePtr<IFFTAlgorithm> FFTAlgorithm = FFFTFactory::NewFFTAlgorithm(FFTSettings);
    
    // 5. FFT 출력 복소수 버퍼 준비
    FAlignedFloatBuffer ComplexOutput;
    ComplexOutput.AddUninitialized(FFTAlgorithm->NumOutputFloats());
    
    // 6. FFT 수행
    FFTAlgorithm->ForwardRealToComplex(InputBuffer.GetData(), ComplexOutput.GetData());
    
    // 7. 주파수 대역별 크기 계산
	{
		FScopeLock Lock(&CriticalSection);
		
		MagnitudeSpectrum.Empty();
		MagnitudeSpectrum.AddUninitialized(FFTSize / 2); // 나이퀴스트 정리에 의해 유효한 주파수 범위는 FFTSize/2
    
		// 실수부와 허수부로부터 크기를 계산
		for (int32 i = 0; i < FFTSize / 2; ++i) {
			const float Real = ComplexOutput[i * 2];
			const float Imaginary = ComplexOutput[i * 2 + 1];
			MagnitudeSpectrum[i] = FMath::Sqrt(Real * Real + Imaginary * Imaginary);
		}
    
		// FFT 스케일링 보정 (EFFTScaling::None 일 경우 크기 0 ~ 1)
		EFFTScaling CurrentScaling = FFTAlgorithm->ForwardScaling();
		ScalePowerSpectrumInPlace(FFTSize, CurrentScaling, EFFTScaling::MultipliedBySqrtFFTSize, MagnitudeSpectrum);	
	}
}

void UMetaAudioCaptureComponent::StartAnalyzingMicrophone(const EFrequencyBandFeatureType Type)
{
	bAnalyzingMicrophone = true;

	FrequencyBandFeature = Type;
}

void UMetaAudioCaptureComponent::StopAnalyzingMicrophone()
{
	bAnalyzingMicrophone = false;
}

void UMetaAudioCaptureComponent::GetMagnitudeForFrequencies(const TArray<float>& InFrequencies, TArray<float>& Magnitudes)
{
	if (!bAnalyzingMicrophone)
	{
		UE_LOG(LogTemp, Warning, TEXT("Please try after calling the StartAnalyzingMicrophone function."));
		return;
	}

	// 주파수 정렬
	TArray<float> Frequencies = InFrequencies;
	Frequencies.Sort();

	// 크기 배열 초기화
	Magnitudes.Empty();
	Magnitudes.Init(0.0f, InFrequencies.Num());

	// 오디오 디바이스
	const FAudioDevice* AudioDevice = GetAudioDevice();
	if (AudioDevice == nullptr) return;

	// FFT 데이터 복사
	Audio::FAlignedFloatBuffer MagnitudeSpectrumCopy;
	{
		FScopeLock Lock(&CriticalSection); // 오디오, 게임 스레드 충돌 방지
		MagnitudeSpectrumCopy = MagnitudeSpectrum;
	}
	if (MagnitudeSpectrumCopy.Num() == 0) return;

	// Sample Rate
	const float SR = AudioDevice->GetSampleRate();
	if (FFTSize == 0 || SR == 0) return;

	// FrequencyResolution
	const float FrequencyResolution = SR / FFTSize;

	// 현재 Band에서 Bin을 어디서부터 탐색해야할지 저장하기 위한 변수
	int32 BinSearchStartIndex = 0;

	// 밴드별 크기배열 가공
	for (int32 BandIndex = 0; BandIndex < Frequencies.Num(); ++BandIndex)
	{
		constexpr float MaxRelativeWidth = 0.5f; // 허용되는 상대 대역폭

		const float TargetFrequency = Frequencies[BandIndex];
		const float NextFrequency = BandIndex + 1 >= Frequencies.Num() ? 20000 : Frequencies[BandIndex + 1];
		const float MaxBandWidth = TargetFrequency * MaxRelativeWidth;
		const float FrequencyThreshold = FMath::Min((TargetFrequency + NextFrequency) / 2, TargetFrequency + MaxBandWidth);

		// 각 분류 방법에 맞게 주파수 대역별 크기 대입
		switch (FrequencyBandFeature)
		{
		case EFrequencyBandFeatureType::Peak:
			for (int32 BinIndex = BinSearchStartIndex; BinIndex < MagnitudeSpectrumCopy.Num(); ++BinIndex)
			{
				// MagnitudeSpectrum 요소의 주파수
				const float BinFrequency = BinIndex * FrequencyResolution;

				if (BinFrequency > FrequencyThreshold)
				{
					BinSearchStartIndex = BinIndex;
					break;
				}

				// 너무 많이 차이나는 주파수 크기는 제외
				if (FMath::Abs(BinFrequency - TargetFrequency) > MaxBandWidth)
				{
					continue;
				}

				if (Magnitudes[BandIndex] < MagnitudeSpectrumCopy[BinIndex])
				{
					Magnitudes[BandIndex] = MagnitudeSpectrumCopy[BinIndex];
				}
			}
			break;
		case EFrequencyBandFeatureType::Mean:
			{
				float BandWeightedTotalMagnitude = 0;
				float TotalWeight = 0;
				
				for (int32 BinIndex = BinSearchStartIndex; BinIndex < MagnitudeSpectrumCopy.Num(); ++BinIndex)
				{
					// MagnitudeSpectrum 요소의 주파수
					const float BinFrequency = BinIndex * FrequencyResolution;

					if (BinFrequency > FrequencyThreshold)
					{
						BinSearchStartIndex = BinIndex;
						
						Magnitudes[BandIndex] = FMath::IsNearlyZero(TotalWeight) ? 0 : BandWeightedTotalMagnitude / TotalWeight;
						break;
					}

					// 너무 많이 차이나는 주파수 크기는 제외
					if (FMath::Abs(BinFrequency - TargetFrequency) > MaxBandWidth)
					{
						continue;
					}

					if (MagnitudeSpectrumCopy[BinIndex] == 0)
					{
						continue;
					}
					
					const float Weight = 1.0f / (1.0f + FMath::Abs(BinFrequency - TargetFrequency));  // 중심에서 멀어질수록 가중치 감소
					const float MagnitudeWithWeight =  MagnitudeSpectrumCopy[BinIndex] * Weight;
					if (MagnitudeWithWeight > 0.0f)
					{
						BandWeightedTotalMagnitude += MagnitudeWithWeight;
						TotalWeight += Weight;	
					}

					UE_LOG(LogTemp, Warning, TEXT("[%d] BinIndex : %d"), BandIndex, BinIndex);
				}
			}
			break;
		case EFrequencyBandFeatureType::Nearest:
			const float NearestValue = FMath::Clamp(TargetFrequency / FrequencyResolution, 0, MagnitudeSpectrumCopy.Num() - 1);
			const int32 FloorIndex = FMath::Floor(NearestValue);
			
			if (FMath::IsNearlyEqual(NearestValue, FloorIndex))
			{
				Magnitudes[BandIndex] = MagnitudeSpectrumCopy[FloorIndex];
			}
			else
			{
				Magnitudes[BandIndex] = FMath::Lerp(MagnitudeSpectrumCopy[FloorIndex], MagnitudeSpectrumCopy[FloorIndex + 1], FMath::Fmod(NearestValue, 1.0f));
			}
			break;
		}
	}
}