#include "PMSeqManager.h"

#include "JsonObjectConverter.h"
#include "PMAppManager.h"
#include "PMLogMacros.h"
#include "Kismet/GameplayStatics.h"

UPMSeqManager* UPMSeqManager::Singleton = nullptr;

const FString ToString(EPMLogLevel e)
{
	switch (e)
	{
	case EPMLogLevel::LOG: return TEXT("LOG");
	case EPMLogLevel::CHECK: return TEXT("CHECK");
	case EPMLogLevel::CROSS: return TEXT("CROSS");
	case EPMLogLevel::WARN: return TEXT("WARN");
	case EPMLogLevel::ERROR: return TEXT("ERROR");
	default: return TEXT("unknown");
	}
}

const FString ToString(EPMSeqLogLevel e)
{
	switch (e)
	{
	case EPMSeqLogLevel::Verbose: return TEXT("Verbose");
	case EPMSeqLogLevel::Debug: return TEXT("Debug");
	case EPMSeqLogLevel::Information: return TEXT("Information");
	case EPMSeqLogLevel::Warning: return TEXT("Warning");
	case EPMSeqLogLevel::Error: return TEXT("Error");
	case EPMSeqLogLevel::Fatal: return TEXT("Fatal");
	default: return TEXT("unknown");
	}
}

UPMSeqManager::UPMSeqManager()
{
	PM_LINE();
	PM_CHECK("UPMSeqManager Constructor (%d)", this);
	PM_LINE();
}

UPMSeqManager::~UPMSeqManager()
{
	PM_LINE();
	PM_CROSS("UPMSeqManager ~Destructor (%d)", this);
	PM_LINE();

	if (Singleton != nullptr)
	{
		Singleton = nullptr;
	}
}

UPMSeqManager* UPMSeqManager::Get()
{
	if (Singleton == nullptr)
	{
		check(IsInGameThread());
		Singleton = NewObject<UPMSeqManager>();
	}
	return Singleton;
}

void UPMSeqManager::InvalidateInstance()
{
	Singleton = nullptr;
}

void UPMSeqManager::Log(const FString& Text, const EPMLogLevel PMLogLevel, bool bPrintToScreen, bool bSendToServer)
{
	FString FinalText = "[BP] " + Text;
	if (bPrintToScreen)
	{
		switch (PMLogLevel) {
		case EPMLogLevel::LOG:		PM_SCREEN_LOG("%s", *FinalText);	break;
		case EPMLogLevel::CHECK:	PM_SCREEN_CHECK("%s", *FinalText);	break;
		case EPMLogLevel::CROSS:	PM_SCREEN_CROSS("%s", *FinalText);	break;
		case EPMLogLevel::WARN:		PM_SCREEN_WARN("%s", *FinalText);	break;
		case EPMLogLevel::ERROR:	PM_SCREEN_ERROR("%s", *FinalText);	break;
		}
	}
	else
	{
		switch (PMLogLevel) {
		case EPMLogLevel::LOG:		PM_LOG("%s", *FinalText);	break;
		case EPMLogLevel::CHECK:	PM_CHECK("%s", *FinalText);	break;
		case EPMLogLevel::CROSS:	PM_CROSS("%s", *FinalText);	break;
		case EPMLogLevel::WARN:		PM_WARN("%s", *FinalText);	break;
		case EPMLogLevel::ERROR:	PM_ERROR("%s", *FinalText);	break;
		}
	}
	if (bSendToServer)
	{
		EPMSeqLogLevel SeqLogLevel = EPMSeqLogLevel::Debug;
		switch (PMLogLevel) {
		case EPMLogLevel::LOG:		SeqLogLevel = EPMSeqLogLevel::Debug;	break;
		case EPMLogLevel::CHECK:	SeqLogLevel = EPMSeqLogLevel::Information;	break;
		case EPMLogLevel::CROSS:	SeqLogLevel = EPMSeqLogLevel::Information;	break;
		case EPMLogLevel::WARN:		SeqLogLevel = EPMSeqLogLevel::Warning;	break;
		case EPMLogLevel::ERROR:	SeqLogLevel = EPMSeqLogLevel::Error;	break;
		}
		Get()->Log(FinalText, SeqLogLevel, "", 0);	
	}
}

void UPMSeqManager::SeqLog(const FString& Text, const EPMSeqLogLevel SeqLogLevel, const TMap<FString, FString>& Params, bool bPrintToLog, bool bPrintToScreen, bool bSendToServer)
{
	FString FinalText = "[BP] " + Text;
	if (bPrintToScreen)
	{
		switch (SeqLogLevel) {
		case EPMSeqLogLevel::Verbose:
		case EPMSeqLogLevel::Debug:
		case EPMSeqLogLevel::Information:
			PM_SCREEN_LOG("%s", *FinalText);
			break;
		case EPMSeqLogLevel::Warning:
			PM_SCREEN_WARN("%s", *FinalText);
			break;
		case EPMSeqLogLevel::Error:
		case EPMSeqLogLevel::Fatal:
			PM_SCREEN_ERROR("%s", *FinalText);
			break;
		}
	}
	else if (bPrintToLog)
	{
		switch (SeqLogLevel) {
		case EPMSeqLogLevel::Verbose:
		case EPMSeqLogLevel::Debug:
		case EPMSeqLogLevel::Information:
			PM_LOG("%s", *FinalText);
			break;
		case EPMSeqLogLevel::Warning:
			PM_WARN("%s", *FinalText);
			break;
		case EPMSeqLogLevel::Error:
		case EPMSeqLogLevel::Fatal:
			PM_ERROR("%s", *FinalText);
			break;
		}
	}
	if (bSendToServer)
	{
		Get()->Log(FinalText, SeqLogLevel, "", 0, Params);	
	}
}

void UPMSeqManager::Log(const FString& Text, const EPMSeqLogLevel SeqLogLevel, const FString& Function, int Line, const TMap<FString, FString>& Params)
{
	const bool bLog = false;
	
	// PM_LOG("Text : %s", *Text);
	// PM_LOG("Function : %s", *Function);
	// PM_LOG("Line : %d", Line);
	
	TMap<FString, FString> Headers = {
		{"Content-Type", "application/vnd.serilog.clef"},
		{"X-Seq-ApiKey", ApiKey}
	};
	const auto URL = FString::Printf(TEXT("%s/api/events/raw?clef"), *Host);

	auto Now = FDateTime::UtcNow();
	if (Now == LastSendDateTime)
	{
		++SendID;
	}
	else
	{
		SendID = 0;
	}
	LastSendDateTime = Now;
	Now += FTimespan(SendID * 10000);
	 
	
	FString JsonString;

	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("@t", Now.ToIso8601());
	JsonObject->SetStringField("@l", ToString(SeqLogLevel));
	JsonObject->SetStringField("@mt", Text);
	if (!Function.IsEmpty()) JsonObject->SetStringField("Function", Function);
	if (Line != 0) JsonObject->SetNumberField("Line", Line);
	JsonObject->SetStringField("ProjectVersion", UPMAppManager::GetProjectVersion());
	JsonObject->SetStringField("ProjectName", FApp::GetProjectName());
	JsonObject->SetStringField("ExecutableName", FPlatformProcess::ExecutableName());
	JsonObject->SetStringField("ComputerName", FPlatformProcess::ComputerName());
	JsonObject->SetStringField("UserName", FPlatformProcess::UserName());
	JsonObject->SetStringField("CurrentWorkingDirectory", FPlatformProcess::GetCurrentWorkingDirectory());
	JsonObject->SetStringField("IP", UPMNetManager::GetMyIpAddress());
	JsonObject->SetStringField("PlatformName", UGameplayStatics::GetPlatformName());
	
#if UE_BUILD_DEBUG
	JsonObject->SetStringField("Build", "Debug");
#elif UE_BUILD_DEVELOPMENT
	JsonObject->SetStringField("Build", "Development");
#else
	JsonObject->SetStringField("Build", "Shipping");
#endif
	
	
	for (auto Param : Params)
	{
		JsonObject->SetStringField(Param.Key, Param.Value);
	}

	// const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonString);
	if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
	{
		// PM_LOG(">> JsonString : %s", *JsonString);
		UPMNetManager::Request(URL, EHttpMethod::POST, Headers, JsonString, nullptr, nullptr, bLog);
	}

	// json deserialize
	// TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	// if (FJsonSerializer::Deserialize(Reader, JsonObject))
	// {
	// }
}