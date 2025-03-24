// Fill out your copyright notice in the Description page of Project Settings.


#include "MultMediaPlayerTestActor.h"

#include "MediaPlayer.h"
#include "MediaTexture.h"
#include "MultiMediaSyncComponent.h"

// Sets default values
AMultMediaPlayerTestActor::AMultMediaPlayerTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Default Scene Component"));
	SetRootComponent(DefaultSceneComponent);

	MultiMediaSyncComponent = CreateDefaultSubobject<UMultiMediaSyncComponent>(TEXT("Multi Media Sync Component"));
}

// Called when the game starts or when spawned
void AMultMediaPlayerTestActor::BeginPlay()
{
	Super::BeginPlay();

	MultiMediaSyncComponent->bLoop = bLoop;
	MultiMediaSyncComponent->OnChangedMultiMediaState.AddDynamic(this, &AMultMediaPlayerTestActor::ChangedMediaState);
}

void AMultMediaPlayerTestActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	MultiMediaSyncComponent->OnChangedMultiMediaState.RemoveDynamic(this, &AMultMediaPlayerTestActor::ChangedMediaState);
	OnPlayerStateChangeEvent.Clear();

	for (auto& MediaPlayer : MediaPlayers) { MediaPlayer = nullptr; }
	for (auto& MediaTexture : MediaTextures) { MediaTexture = nullptr; }
	MediaPlayers.Empty();
	MediaTextures.Empty();
}

void AMultMediaPlayerTestActor::Ready()
{
	for (int32 i = 0; i < MediaSources.Num(); ++i)
	{
		MediaPlayers.Emplace(NewObject<UMediaPlayer>(this));
		MediaTextures.Emplace(NewObject<UMediaTexture>(this));
		
		FMediaPlayerStruct MediaPlayerStruct;
		MediaPlayerStruct.MediaPlayer = MediaPlayers[i];
		MediaPlayerStruct.MediaTexture = MediaTextures[i];
		MediaPlayerStruct.FileMediaSource = MediaSources[i];
		MultiMediaSyncComponent->MediaPlayerStructs.Emplace(MediaPlayerStruct);
	}

	MultiMediaSyncComponent->Prepare();
}

void AMultMediaPlayerTestActor::Play()
{
	bPlayRequest = true;
	if (MultiMediaSyncComponent->GetMultiMediaState() == EMultiMediaState::None)
	{
		Ready();
	}
	else if (MultiMediaSyncComponent->GetMultiMediaState() == EMultiMediaState::Ready)
	{
		MultiMediaSyncComponent->Play();
	}
}

void AMultMediaPlayerTestActor::Stop()
{
	if (MultiMediaSyncComponent->GetMultiMediaState() != EMultiMediaState::Playing) return;
	
	MultiMediaSyncComponent->Pause();
}

void AMultMediaPlayerTestActor::ChangedMediaState(const EMultiMediaState NewState)
{
	if (OnPlayerStateChangeEvent.IsBound())
	{
		OnPlayerStateChangeEvent.Broadcast(NewState);
	}
	
	if (bPlayRequest && NewState == EMultiMediaState::Ready)
	{
		bPlayRequest = false;
		MultiMediaSyncComponent->Play();
	}
}

