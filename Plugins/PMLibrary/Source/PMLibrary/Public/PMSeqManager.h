// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PMLogMacros.h"
#include "PMNetManager.h"
#include "UObject/Object.h"
#include "PMSeqManager.generated.h"

UENUM(BlueprintType)
enum class EPMLogLevel : uint8
{
	LOG UMETA(DisplayName = "LOG"),
	CHECK UMETA(DisplayName = "CHECK"),
	CROSS UMETA(DisplayName = "CROSS"),
	WARN UMETA(DisplayName = "WARN"),
	ERROR UMETA(DisplayName = "ERROR"),
};
const FString ToString(EPMLogLevel e);

UENUM(BlueprintType)
enum class EPMSeqLogLevel : uint8
{
	Verbose UMETA(DisplayName = "Verbose"),
	Debug UMETA(DisplayName = "Debug"),
	Information UMETA(DisplayName = "Information"),
	Warning UMETA(DisplayName = "Warning"),
	Error UMETA(DisplayName = "Error"),
	Fatal UMETA(DisplayName = "Fatal"),
};
const FString ToString(EPMSeqLogLevel e);

/**
 * 
 */
UCLASS()
class PMLIBRARY_API UPMSeqManager : public UObject
{
	GENERATED_BODY()
	
private:
	UPMSeqManager();
	virtual ~UPMSeqManager() override;
	
	static UPMSeqManager* Singleton;
	inline static FDateTime LastSendDateTime;
	inline static int SendID;
	
public:
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Get PMSeqManager"), Category="PostMedia")
	static UPMSeqManager* Get();

	UFUNCTION(BlueprintCallable, Category="PostMedia")
	static void InvalidateInstance();

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Config")
	FString Host;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Config")
	FString ApiKey;

	UFUNCTION(BlueprintCallable, meta=(DisplayName = "PM Log", AdvancedDisplay = "1"), Category="PostMedia")
	static void Log(const FString& Text, const EPMLogLevel LogLevel, bool bPrintToScreen = true, bool bSendToServer = false);

	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="Params", DisplayName = "PM Seq Log", AdvancedDisplay = "2"), Category="PostMedia")
	static void SeqLog(const FString& Text, const EPMSeqLogLevel SeqLogLevel, const TMap<FString, FString>& Params, bool bPrintToLog = true, bool bPrintToScreen = true, bool bSendToServer = true);
	void Log(const FString& Text, const EPMSeqLogLevel SeqLogLevel, const FString& Function, int Line, const TMap<FString, FString>& Params = {});

	// template <typename FmtType, typename... Types>
	// UE_NODISCARD static void Log(const FmtType& Fmt, Types... Args)
	// {
	// 	TMap<FString, FString> Headers;
	// 	Headers.Add("Content-Type", "application/vnd.serilog.clef");
	// 	Headers.Add("X-Seq-ApiKey", "1h24hySyNPOKq7o2xDuO");
	// 	PMNetManager->Request(TEXT("https://post-seq.duckdns.org/api/events/raw?clef"), EHttpMethod::POST, Headers, TEXT("{\"@t\":\"2022-11-17T16:20:00.8532799Z\",\"@mt\":\"Hello, {User}\",\"User\":\"alice\"}"));
	// }
};
