// Copyright Epic Games, Inc. All Rights Reserved.

#include "PMLibrary.h"

#include "PMLogMacros.h"

#define LOCTEXT_NAMESPACE "FPMLibraryModule"

void FPMLibraryModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	PM_LINE();
}

void FPMLibraryModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	PM_LINE();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPMLibraryModule, PMLibrary)