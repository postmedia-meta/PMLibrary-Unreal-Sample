

#include "MultiMediaSyncComponent.h"

#include "MediaPlayer.h"
#include "MediaTexture.h"
#include "FileMediaSource.h"
#include "TimerManager.h"
#include "Engine/World.h"

UMultiMediaSyncComponent::UMultiMediaSyncComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMultiMediaSyncComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	// 액터가 사라질 때, 미디어 텍스처를 지우기 위함.
	// for (const auto MediaPlayerStruct : MediaPlayerStructs)
	// {
	// 	MediaPlayerStruct.MediaTexture->SetMediaPlayer(nullptr);
	// }

	if (SeekHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(SeekHandle);
		SeekHandle.Invalidate();
	}
	if (SyncHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(SyncHandle);
		SyncHandle.Invalidate();
	}

	for (const auto MediaPlayerStruct : MediaPlayerStructs)
	{
		if (MediaPlayerStruct.MediaPlayer->OnMediaOpened.IsBound()) MediaPlayerStruct.MediaPlayer->OnMediaOpened.RemoveDynamic(this, &UMultiMediaSyncComponent::MediaOpened);
		if (MediaPlayerStruct.MediaPlayer->OnEndReached.IsBound()) MediaPlayerStruct.MediaPlayer->OnEndReached.RemoveDynamic(this, &UMultiMediaSyncComponent::EndReached);
		if (MediaPlayerStruct.MediaPlayer->OnPlaybackSuspended.IsBound()) MediaPlayerStruct.MediaPlayer->OnPlaybackSuspended.RemoveDynamic(this, &UMultiMediaSyncComponent::PlaybackSuspended);
		if (MediaPlayerStruct.MediaPlayer->OnSeekCompleted.IsBound()) MediaPlayerStruct.MediaPlayer->OnSeekCompleted.RemoveDynamic(this, &UMultiMediaSyncComponent::SeekCompleted);	
	}
}

void UMultiMediaSyncComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (State == EMultiMediaState::Preparing)
	{
		bool IsPlaying = true;
		for (const auto MediaPlayerStruct : MediaPlayerStructs)
		{
			if (MediaPlayerStruct.MediaPlayer->GetTime().GetTotalSeconds() <= 0)
			{
				IsPlaying = false;
				break;
			}
		}
	
		if (IsPlaying)
		{
			if (!bFirstPlayStarted)
			{
				UE_LOG(LogTemp, Log, TEXT("[%s] First Play Started!!!"), *GetName());
			
				bFirstPlayStarted = true;
				Pause();	
			}
		}
	}

	if (State == EMultiMediaState::Playing)
	{
		if (!bSyncing && !bSeeking)
		{
			if (bCanSync && GetPlayTimeDifference() > OneFrameSec * FrameOut)
			{
				MediaSync();
			}
			else
			{
				bool bIsFreeze = true;
				for (auto& MediaPlayerStruct : MediaPlayerStructs)
				{
					// freeze 상태 확인
					if (MediaPlayerStruct.MediaPlayer->GetTime() != MediaPlayerStruct.PrevTime)
					{
						MediaPlayerStruct.PrevTime = MediaPlayerStruct.MediaPlayer->GetTime();
						bIsFreeze = false;
					}
				}
				
				FreezeTickCnt = bIsFreeze ? FreezeTickCnt + 1 : 0;
				if (FreezeTickCnt > 30)
				{
					FreezeTickCnt = 0;
					const auto SeekTime = FTimespan::FromSeconds(MediaPlayerStructs[0].MediaPlayer->GetTime().GetTotalSeconds() + OneFrameSec);
					Seek(SeekTime);
				}	
			}
		}
	}
}

void UMultiMediaSyncComponent::Prepare()
{
	StartTime = FPlatformTime::Seconds();
	bFirstPlayStarted = false;
	bSeeking = false;
	
	// 미디어 플레이어 세팅
	for (const auto MediaPlayerStruct : MediaPlayerStructs)
	{
		MediaPlayerStruct.MediaTexture->SetMediaPlayer(MediaPlayerStruct.MediaPlayer);
		MediaPlayerStruct.MediaTexture->UpdateResource();
		
		MediaPlayerStruct.MediaPlayer->PlayOnOpen = true;
		if (!MediaPlayerStruct.MediaPlayer->OnMediaOpened.IsBound()) MediaPlayerStruct.MediaPlayer->OnMediaOpened.AddDynamic(this, &UMultiMediaSyncComponent::MediaOpened);
		if (!MediaPlayerStruct.MediaPlayer->OnEndReached.IsBound()) MediaPlayerStruct.MediaPlayer->OnEndReached.AddDynamic(this, &UMultiMediaSyncComponent::EndReached);
		if (!MediaPlayerStruct.MediaPlayer->OnPlaybackSuspended.IsBound()) MediaPlayerStruct.MediaPlayer->OnPlaybackSuspended.AddDynamic(this, &UMultiMediaSyncComponent::PlaybackSuspended);
		if (!MediaPlayerStruct.MediaPlayer->OnSeekCompleted.IsBound()) MediaPlayerStruct.MediaPlayer->OnSeekCompleted.AddDynamic(this, &UMultiMediaSyncComponent::SeekCompleted);
		
		MediaPlayerStruct.MediaPlayer->SetLooping(false);
		MediaPlayerStruct.MediaPlayer->OpenSource(MediaPlayerStruct.FileMediaSource);
	}

	// 미디어 플레이어 싱크 준비
	ChangeMultiMediaState(EMultiMediaState::Opening);
}

void UMultiMediaSyncComponent::MediaOpened(FString OpenedUrl)
{
	for (const auto MediaPlayerStruct : MediaPlayerStructs)
	{
		if (!MediaPlayerStruct.MediaPlayer->IsReady()) return;
	}

	ChangeMultiMediaState(EMultiMediaState::Preparing);
}

void UMultiMediaSyncComponent::EndReached()
{
	for (const auto MediaPlayerStruct : MediaPlayerStructs)
	{
		if (MediaPlayerStruct.MediaPlayer->IsPlaying()) return;
	}
	
	if (State == EMultiMediaState::Preparing)
	{
		bFirstPlayStarted = true;
		Seek(FTimespan::Zero());
		return;
	}

	if (bLoop) return;
	
	ChangeMultiMediaState(EMultiMediaState::Finished);
}

void UMultiMediaSyncComponent::SeekCompleted()
{
	UE_LOG(LogTemp, Log, TEXT("[%s] SeekCompleted %d/%d"), *GetName(), SeekedCnt + 1, MediaPlayerStructs.Num());
	
	++SeekedCnt;
	if (SeekedCnt < MediaPlayerStructs.Num()) return;
	SeekedCnt = 0;
	
	GetWorld()->GetTimerManager().SetTimer(SeekHandle, FTimerDelegate::CreateLambda([this]()->void
	{
		bSeeking = false;
		bSyncing = false;
	}), 5, false);
	
	if (State == EMultiMediaState::Preparing)
	{
		ElapsedTime = FPlatformTime::Seconds() - StartTime;
		UE_LOG(LogTemp, Log, TEXT("[%s] Preparing elapsed time : %lf"), *GetName(), ElapsedTime);

		for (const auto MediaPlayerStruct : MediaPlayerStructs) { MediaPlayerStruct.MediaPlayer->SetLooping(bLoop); }
		ChangeMultiMediaState(EMultiMediaState::Ready);
		return;
	}
	
	ElapsedTime = FPlatformTime::Seconds() - StartTime;
	UE_LOG(LogTemp, Log, TEXT("[%s] Media sync elapsed time : %lf // StartTime : %lf, EndTime : %lf"), *GetName(), ElapsedTime, StartTime, FPlatformTime::Seconds());

	for (const auto MediaPlayerStruct : MediaPlayerStructs)
	{
		MediaPlayerStruct.MediaPlayer->Play();
	}
}

void UMultiMediaSyncComponent::PlaybackSuspended()
{
	if (bSeeking) return;
	UE_LOG(LogTemp, Log, TEXT("[%s] PlaybackSuspended %d/%d"), *GetName(), PausedCnt + 1, MediaPlayerStructs.Num());
	
	++PausedCnt;
	if (PausedCnt < MediaPlayerStructs.Num()) return;
	PausedCnt = 0;
	
	if (State == EMultiMediaState::Preparing)
	{
		Seek(FTimespan::Zero());
	}
	else if (State == EMultiMediaState::Closing)
	{
		for (const auto MediaPlayerStruct : MediaPlayerStructs)
		{
			MediaPlayerStruct.MediaPlayer->Close();
			ChangeMultiMediaState(EMultiMediaState::Finished);
		}
	}
	else if (bSyncing)
	{
		const auto SeekTime = FTimespan::FromSeconds(MediaPlayerStructs[0].MediaPlayer->GetTime().GetTotalSeconds() + OneFrameSec * FrameOut);
		Seek(SeekTime);
	}
	else
	{
		ChangeMultiMediaState(EMultiMediaState::Paused);
	}
}

void UMultiMediaSyncComponent::Play()
{
	if (State == EMultiMediaState::None)
	{
		Prepare();
		return;
	}
	
	if (State != EMultiMediaState::Ready) return;
	
	for (const auto MediaPlayerStruct : MediaPlayerStructs)MediaPlayerStruct.MediaPlayer->Play();
	ChangeMultiMediaState(EMultiMediaState::Playing);

	GetWorld()->GetTimerManager().SetTimer(SeekHandle, FTimerDelegate::CreateLambda([this]()->void{bCanSync = true;}), 3, false);
}

void UMultiMediaSyncComponent::Seek(const FTimespan SeekTime)
{
	UE_LOG(LogTemp, Log, TEXT("[%s] Seek: %.4f"), *GetName(), SeekTime.GetTotalSeconds());
	
	bSeeking = true;
	for (const auto MediaPlayerStruct : MediaPlayerStructs)
	{
		MediaPlayerStruct.MediaPlayer->Seek(SeekTime);
	}
}

void UMultiMediaSyncComponent::Pause()
{
	for (const auto MediaPlayerStruct : MediaPlayerStructs) MediaPlayerStruct.MediaPlayer->Pause();
}

void UMultiMediaSyncComponent::Close()
{
	if (State == EMultiMediaState::Closing || State == EMultiMediaState::Finished || State == EMultiMediaState::Error) return;
	ChangeMultiMediaState(EMultiMediaState::Closing);
	Pause();
}

void UMultiMediaSyncComponent::MediaSync()
{
	UE_LOG(LogTemp, Log, TEXT("[%s] MediaSync"), *GetName());
	
	StartTime = FPlatformTime::Seconds();
	bSyncing = true;
	Pause();
}

FTimespan UMultiMediaSyncComponent::GetPlayTimeDifference()
{
	FTimespan SlowestPlayTime = FTimespan::MaxValue();
	FTimespan FastestPlayTime = FTimespan::MinValue();
	
	for (const auto MediaPlayerStruct : MediaPlayerStructs)
	{
		const auto PlayTime = MediaPlayerStruct.MediaPlayer->GetTime();
		if (SlowestPlayTime > PlayTime) SlowestPlayTime = PlayTime;
		if (FastestPlayTime < PlayTime) FastestPlayTime = PlayTime;
	}
	
	const auto TimeDiff = FastestPlayTime - SlowestPlayTime;
	return TimeDiff;
}

void UMultiMediaSyncComponent::ChangeMultiMediaState(const EMultiMediaState NewState)
{
	if (State == NewState) return;
	if (const UEnum* CharStateEnum = FindFirstObject<UEnum>(TEXT("EMultiMediaState")))
	{
		FString LogString = FString::Printf(TEXT("[%s] Multi Media state change : %s -> %s"), *GetName(), *CharStateEnum->GetNameStringByIndex(static_cast<uint8>(State)), *CharStateEnum->GetNameStringByIndex(static_cast<uint8>(NewState)));
		UE_LOG(LogTemp, Log, TEXT("%s"), *LogString);
	}
	
	State = NewState;

	// 미디어플레이어 상태 변화에 따른 변화.
	switch (NewState)
	{
		case EMultiMediaState::Preparing:
		{
			// 비디오 플레이어의 총재생시간과 프레임 레이트가 일치하는지 확인하고 1프레임 넘어가는데 걸리는 시간과 영상이 끝나야할 시간을 결정.
			float FrameRate = MediaPlayerStructs[0].MediaPlayer->GetVideoTrackFrameRate(0, 0);
			DurationSec = MediaPlayerStructs[0].MediaPlayer->GetDuration().GetTotalSeconds();
			for (auto& MediaPlayerStruct : MediaPlayerStructs)
			{
				if (FrameRate != MediaPlayerStruct.MediaPlayer->GetVideoTrackFrameRate(0, 0))
				{
					UE_LOG(LogTemp, Error, TEXT("[%s] The frame rates are not the same!!!"), *GetName());
					ChangeMultiMediaState(EMultiMediaState::Error);
					return;
				}
				if (DurationSec != MediaPlayerStruct.MediaPlayer->GetDuration().GetTotalSeconds())
				{
					UE_LOG(LogTemp, Error, TEXT("[%s] Duration is not the same!!!"), *GetName());
					ChangeMultiMediaState(EMultiMediaState::Error);
					return;
				}
			}
			OneFrameSec = 1 / FrameRate;
			break;
		}
		case EMultiMediaState::Finished:
		{
			if (!MediaPlayerStructs[0].MediaPlayer->IsClosed())
			{
				for (const auto MediaPlayerStruct : MediaPlayerStructs) MediaPlayerStruct.MediaPlayer->Close();
			}
			break;	
		}
		default: ;
	}

	// state가 변화하면 이벤트 발생
	if (OnChangedMultiMediaState.IsBound()) OnChangedMultiMediaState.Broadcast(NewState);
}