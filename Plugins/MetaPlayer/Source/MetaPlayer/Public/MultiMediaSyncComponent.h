#pragma once

#include "CoreMinimal.h"
#include "MultiMediaSyncComponent.generated.h"

class UMediaPlayer;
class UMediaTexture;
class UFileMediaSource;

UENUM(BlueprintType)
enum class EMultiMediaState : uint8
{
	None = 0,
	Opening,
	Preparing,
	Ready,
	Playing,
	Paused,
	Finished,
	Closing,
	Error
};

USTRUCT(BlueprintType)
struct FMediaPlayerStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Multi Media")
	UMediaPlayer* MediaPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Multi Media")
	UMediaTexture* MediaTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Multi Media")
	UFileMediaSource* FileMediaSource;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Multi Media")
	FTimespan PrevTime = FTimespan::Zero();
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangedMultiMediaState, EMultiMediaState, NewState);

UCLASS(ClassGroup = "Media", meta = (BlueprintSpawnableComponent))
class METAPLAYER_API UMultiMediaSyncComponent : public UActorComponent
{
	GENERATED_BODY()

	UMultiMediaSyncComponent();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Multi Media")
	TArray<FMediaPlayerStruct> MediaPlayerStructs;
	
	// 비디오를 루핑할지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Multi Media")
	bool bLoop = false;

	// 실시간으로 영상의 싱크를 맞출지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Multi Media")
	bool bAutoSync = true;
	
	// 몇 프레임이 넘으면 싱크를 할지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Multi Media", meta = (EditCondition = "bAutoSync"))
	int32 FrameOut = 3;
	
	// State change event
	UPROPERTY(BlueprintAssignable, Category="Multi Media|Event")
	FOnChangedMultiMediaState OnChangedMultiMediaState;
	
private:
	double DurationSec = 0;
	float OneFrameSec = 0;

	double StartTime = 0;
	double ElapsedTime = 0;

	int32 PausedCnt = 0;
	int32 SeekedCnt = 0;
	int32 FreezeTickCnt = 0;

	bool bFirstPlayStarted = false;
	bool bCanSync = false;
	bool bSeeking = false;
	bool bSyncing = false;
	
	EMultiMediaState State = EMultiMediaState::None;
	
	FTimerHandle SeekHandle;
	FTimerHandle SyncHandle;

public:
	UFUNCTION(BlueprintCallable, Category="Multi Media")
	void Prepare();
	
	UFUNCTION(BlueprintCallable, Category="Multi Media")
	void Play();

	UFUNCTION(BlueprintCallable, Category="Multi Media")
	void Seek(const FTimespan SeekTime);
	
	UFUNCTION(BlueprintCallable, Category="Multi Media")
	void Pause();
	
	UFUNCTION(BlueprintCallable, Category="Multi Media")
	void Close();

	UFUNCTION(BlueprintCallable, Category="Multi Media")
	void MediaSync();

	// 미디어플레이어 재생 중 가장 큰 시간차를 알기 위함.
	UFUNCTION(BlueprintCallable, Category="Multi Media")
	FTimespan GetPlayTimeDifference();

	// 현재 상태 반환.
	UFUNCTION(BlueprintCallable, Category="Multi Media")
	EMultiMediaState GetMultiMediaState() const { return State; }

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UFUNCTION()
	void ChangeMultiMediaState(const EMultiMediaState NewState);
	
	// 각 미디어플레이어가 재생되면 발생하는 이벤트
	UFUNCTION()
	void MediaOpened(FString OpenedUrl);

	UFUNCTION()
	void EndReached();
	
	UFUNCTION()
	void SeekCompleted();

	UFUNCTION()
	void PlaybackSuspended();
};