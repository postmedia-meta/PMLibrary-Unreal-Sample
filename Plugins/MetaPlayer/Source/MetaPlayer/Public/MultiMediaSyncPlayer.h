// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MultiMediaSyncPlayer.generated.h"

class UFileMediaSource;
class UMultiMediaSyncComponent;
class UMediaPlayer;
class UMediaTexture;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChangeEvent, const EMultiMediaState, MultiMediaState);

UCLASS()
class METAPLAYER_API AMultiMediaSyncPlayer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMultiMediaSyncPlayer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Component")
	UMultiMediaSyncComponent* MultiMediaSyncComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Media|Player")
	TArray<UFileMediaSource*> MediaSources;

	UPROPERTY(BlueprintAssignable, Category="Media|Event")
	FOnPlayerStateChangeEvent OnPlayerStateChangeEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Media|Player")
	bool bLoop;

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
	UFUNCTION(BlueprintCallable)
	void Ready();

	UFUNCTION(BlueprintCallable)
	void Play();

	UFUNCTION(BlueprintCallable)
	void Stop();

private:
	UFUNCTION()
	void ChangedMediaState(const EMultiMediaState NewState);

public:
	UFUNCTION(BlueprintCallable)
	TArray<UMediaTexture*> GetMediaTextures() {return MediaTextures;}
};
