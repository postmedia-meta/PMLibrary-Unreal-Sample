// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PMNetManager.generated.h"

// DECLARE_DYNAMIC_DELEGATE_OneParam			// 1:1
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam	// 1:N
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnResponse, const FString&, Response);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnError, const FString&, Status);

UENUM(BlueprintType)
enum class EHttpMethod : uint8
{
	GET UMETA(DisplayName = "GET"),
	PUT UMETA(DisplayName = "PUT"),
	POST UMETA(DisplayName = "POST"),
	HEAD UMETA(DisplayName = "HEAD"),
	DELETE UMETA(DisplayName = "DELETE"),
};
const FString ToString(EHttpMethod e);

/**
 * 
 */
UCLASS()
class PMLIBRARY_API UPMNetManager : public UObject
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="Headers, OnResponse, OnError", DisplayName = "PM HTTP Request", AdvancedDisplay = "2"), Category="PostMedia")
	static void Request(const FString& URL, const EHttpMethod Method, const TMap<FString, FString>& Headers, const FString& ContentString, const FOnResponse& OnResponse, const FOnError& OnError, const bool bPrintToLog = true);
	static void Request(const FString& URL, const EHttpMethod Method = EHttpMethod::GET, const TMap<FString, FString>& Headers = {}, const FString& ContentString = "", TFunction<void(const FString& Response)>&& OnResponse = nullptr, TFunction<void(const FString& Status)>&& OnError = nullptr, const bool bPrintToLog = true);

	UFUNCTION(BlueprintPure, Category="PostMedia")
	static FString GetMyIpAddress();

private:
	inline static FString MyIpAddress = "NONE";
};
