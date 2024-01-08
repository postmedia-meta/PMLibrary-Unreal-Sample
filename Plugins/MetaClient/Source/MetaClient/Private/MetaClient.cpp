// Copyright Epic Games, Inc. All Rights Reserved.

#include "MetaClient.h"

#define LOCTEXT_NAMESPACE "FMetaClientModule"

void FMetaClientModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FMetaClientModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMetaClientModule, MetaClient)