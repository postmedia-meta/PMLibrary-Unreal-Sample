// Fill out your copyright notice in the Description page of Project Settings.


#include "PMNetManager.h"

#include "HttpModule.h"
#include "PMLogMacros.h"
#include "SocketSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

const FString ToString(EHttpMethod e)
{
	switch (e)
	{
	case EHttpMethod::GET: return TEXT("GET");
	case EHttpMethod::PUT: return TEXT("PUT");
	case EHttpMethod::POST: return TEXT("POST");
	case EHttpMethod::HEAD: return TEXT("HEAD");
	case EHttpMethod::DELETE: return TEXT("DELETE");
	default: return TEXT("unknown");
	}
}

void UPMNetManager::Request(const FString& URL, const EHttpMethod Method, const TMap<FString, FString>& Headers, const FString& ContentString, const FOnResponse& OnResponse, const FOnError& OnError, const bool bPrintToLog)
{
	Request(URL, Method, Headers, ContentString,
	[OnResponse](const FString& Response)
	{
		OnResponse.ExecuteIfBound(Response);
	},
	[OnError](const FString& ErrorMessage)
	{
		OnError.ExecuteIfBound(ErrorMessage);
	},
	bPrintToLog);
}

void UPMNetManager::Request(const FString& URL, const EHttpMethod Method, const TMap<FString, FString>& Headers, const FString& ContentString, TFunction<void(const FString& Response)>&& OnResponse, TFunction<void(const FString& Status)>&& OnError, const bool bPrintToLog)
{
	if (bPrintToLog)
	{
		PM_LINE();
		PM_LOG("[Request] %s :: %s", *ToString(Method), *URL);
		PM_LINE();	
	}

	FHttpModule& httpModule = FHttpModule::Get();

	// Create Request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = httpModule.CreateRequest();

	// Set Method
	pRequest->SetVerb(ToString(Method));

	// Set Header (Default)
	pRequest->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	
	// Set Header
	for (auto Header : Headers)
	{
		pRequest->SetHeader(Header.Key, Header.Value);	
	}

	// Set Content
	pRequest->SetContentAsString(ContentString);

	// Set URL
	pRequest->SetURL(URL);

	// Bind Event
	pRequest->OnProcessRequestComplete().BindLambda([=](FHttpRequestPtr pReq, FHttpResponsePtr pRes, bool connectedSuccessfully)  
	{
		if (connectedSuccessfully)
		{
			const auto Response = FString(pRes->GetContentAsString());
			if (bPrintToLog)
			{
				PM_LINE();
				PM_LOG("[Response] %s :: %s\n%s", *ToString(Method), *URL, *Response);
				PM_LINE();	
			}
			if (OnResponse != nullptr) OnResponse(Response);
		}
		else
		{
			const auto Status = ToString(pReq->GetStatus());
			if (bPrintToLog)
			{
				PM_LINE();
				PM_ERROR("[Response][Error] %s :: %s\n%s", *ToString(Method), *URL, *Status);
				PM_LINE();	
			}
			if (OnError != nullptr) OnError(Status);		
		}
	});

	// Request
	pRequest->ProcessRequest();
}

FString UPMNetManager::GetMyIpAddress()
{
	if (MyIpAddress.Equals("NONE"))
	{
		bool canBind = false;
		TSharedRef<FInternetAddr> LocalIp = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, canBind);
		if (LocalIp->IsValid())
		{
			MyIpAddress = LocalIp->ToString(false);
		}	
	}
	return MyIpAddress;
}