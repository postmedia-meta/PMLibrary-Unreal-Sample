// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MTKPlayerController.generated.h"

class UMetaGraphicsSettingsComponent;

UCLASS()
class METATOOLKIT_API AMTKPlayerController : public APlayerController
{
	GENERATED_BODY()

	AMTKPlayerController();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaToolKit")
	UMetaGraphicsSettingsComponent* MetaGraphicsSettingsComponent;

	virtual void SetupInputComponent() override;

public:
	UFUNCTION(BlueprintCallable, Category="MetaToolKit")
	virtual void Escape();
};
