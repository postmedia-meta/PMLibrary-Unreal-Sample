// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

#include "PMLogMacros.h"

void UMyGameInstance::Init()
{
	Super::Init();
	PM_LINE();
	PM_LOG("GI - Init");
	PM_LINE();


	//
	// Editor Mode
	//
	#if WITH_EDITOR
	PM_LINE()
	PM_LOG("---------- EDITOR MODE ----------")
	PM_LINE()

	// GC
	PM_CHECK("---------- ForceGarbageCollection ----------")
	GEngine->ForceGarbageCollection(true);

	// Managers - InvalidateInstance
	PM_CHECK("---------- Managers InvalidateInstance ----------")
	UPMSeqManager::InvalidateInstance();

	PM_LINE()
#endif

	//
	// PM Log - Example
	//
	PM_LOG("LOG");
	PM_CHECK("CHECK");
	PM_CROSS("CROSS");
	PM_WARN("WARN");
	PM_ERROR("ERROR");

	PM_SCREEN_LOG("PM_SCREEN_LOG");
	PM_SCREEN_CHECK("PM_SCREEN_CHECK");
	PM_SCREEN_CROSS("PM_SCREEN_CROSS");
	PM_SCREEN_WARN("PM_SCREEN_WARN");
	PM_SCREEN_ERROR("PM_SCREEN_ERROR");
	
	// PM_SCREEN_LOG_T(5.0, "PM_SCREEN_LOG_T");
	// PM_SCREEN_CHECK_T(5.0, "PM_SCREEN_CHECK_T");
	// PM_SCREEN_CROSS_T(5.0, "PM_SCREEN_CROSS_T");
	// PM_SCREEN_WARN_T(5.0, "PM_SCREEN_WARN_T");
	// PM_SCREEN_ERROR_T(5.0, "PM_SCREEN_ERROR_T");
	
	// PM_SCREEN_MSG(FColor::Purple, 5.0, "PM_SCREEN_MSG");

	
	//
	// Managers
	//
	PMSeqManager = UPMSeqManager::Get();

	
	//
	// Seq
	//
	UPMSeqManager::Get()->Host = TEXT("https://post-seq.duckdns.org");
	UPMSeqManager::Get()->ApiKey = TEXT("1h24hySyNPOKq7o2xDuO");
	// UPMSeqManager::Get()->Host = TEXT("http://localhost:5341");
	PM_SEQ_CHECK("%s ==================================================", FApp::GetProjectName());
}

void UMyGameInstance::OnStart()
{
	Super::OnStart();
	PM_LINE();
	PM_LOG("GI - OnStart")
	PM_LINE();
}
