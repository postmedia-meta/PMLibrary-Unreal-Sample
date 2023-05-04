// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PMSeqManager.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PMLIBRARYSAMPLE_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void OnStart() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Managers")
	UPMSeqManager *PMSeqManager;
};
