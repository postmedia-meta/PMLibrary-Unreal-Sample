// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiMediaSyncPlayer.h"
#include "GameFramework/Actor.h"
#include "MultMediaPlayerTestActor.generated.h"

UCLASS()
class METAPLAYER_API AMultMediaPlayerTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMultMediaPlayerTestActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MultiMediaSyncPlayer")
	USceneComponent* DefaultSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MultiMediaSyncPlayer")
	UMultiMediaSyncComponent* MultiMediaSyncComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MultiMediaSyncPlayer")
	TArray<UFileMediaSource*> MediaSources;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MultiMediaSyncPlayer")
	bool bLoop;

	UPROPERTY(BlueprintAssignable, Category="Media|Event")
	FOnPlayerStateChangeEvent OnPlayerStateChangeEvent;

private:
	UPROPERTY()
	bool bPlayRequest;
	
	UPROPERTY()
	TArray<UMediaPlayer*> MediaPlayers;

	UPROPERTY()
	TArray<UMediaTexture*> MediaTextures;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintCallable, Category="MultiMediaSyncPlayer")
	void Ready();

	UFUNCTION(BlueprintCallable, Category="MultiMediaSyncPlayer")
	void Play();

	UFUNCTION(BlueprintCallable, Category="MultiMediaSyncPlayer")
	void Stop();

private:
	UFUNCTION()
	void ChangedMediaState(const EMultiMediaState NewState);

public:
	UFUNCTION(BlueprintCallable)
	TArray<UMediaTexture*> GetMediaTextures() {return MediaTextures;}

};
