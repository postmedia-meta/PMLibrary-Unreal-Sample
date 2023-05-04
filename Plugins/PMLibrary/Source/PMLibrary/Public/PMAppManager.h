// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PMAppManager.generated.h"

/**
 * 
 */
UCLASS()
class PMLIBRARY_API UPMAppManager : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static FString GetProjectVersion();
};
