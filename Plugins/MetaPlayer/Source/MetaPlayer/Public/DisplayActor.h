// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiMediaSyncComponent.h"
#include "GameFramework/Actor.h"
#include "DisplayActor.generated.h"

class UArrowComponent;
class AMultiMediaSyncPlayer;
class UMediaTexture;

UCLASS()
class METAPLAYER_API ADisplayActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADisplayActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Display")
	AMultiMediaSyncPlayer* MultiMediaSyncPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Component")
	UArrowComponent* ArrowComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Component")
	TArray<UStaticMeshComponent*> Displays;

private:
	UPROPERTY(EditAnywhere, Category="Display|Material")
	UMaterial* DisplayMaterial;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> MaterialInstanceDynamics;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category="DisplayWidget")
	void SetMediaTexturesDisplayMaterials(const TArray<UMediaTexture*>& MediaTextures);

private:
	UFUNCTION()
	void OnPlayerStateChange(const EMultiMediaState MultiMediaState);
};
