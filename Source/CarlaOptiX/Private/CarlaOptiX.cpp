// Copyright Epic Games, Inc. All Rights Reserved.

#include "CarlaOptiX.h"
#include "OptiX_Instance.h"

#define LOCTEXT_NAMESPACE "FCarlaOptiXModule"

void FCarlaOptiXModule::StartupModule()
{
#ifndef CARLA_OPTIX_NO_AUTO_GLOBAL_INIT
	FCarlaOptiXInstance::InitGlobalContext();
#endif
}

void FCarlaOptiXModule::ShutdownModule()
{
#ifndef CARLA_OPTIX_NO_AUTO_GLOBAL_UNINIT
	FCarlaOptiXInstance::DestroyGlobalContext();
#endif
}

#undef LOCTEXT_NAMESPACE

thread_local char LogBuffer[4096];

IMPLEMENT_MODULE(FCarlaOptiXModule, CarlaOptiX)
DEFINE_LOG_CATEGORY(LogCarlaOptiX)