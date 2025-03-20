// Copyright Epic Games, Inc. All Rights Reserved.

#include "MetaToolkit.h"

#define LOCTEXT_NAMESPACE "FMetaToolkitModule"

void FMetaToolkitModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FMetaToolkitModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMetaToolkitModule, MetaToolkit)