#pragma once
#include "OptiX_Common.h"
#include "OptiX_Module.h"
#include <span>



class CARLAOPTIX_API FCarlaOptiXInstance;



class CARLAOPTIX_API FCarlaOptiXProgramGroup
{
	OptixProgramGroup Handle = OptixProgramGroup();

public:

	static OptixProgramGroupDesc MakeRayGenProgGroupDescription(
		OptixModule ModuleHandle,
		const char* EntryPoint = "__raygen__main");

	static OptixProgramGroupDesc MakeMissProgGroupDescription(
		OptixModule ModuleHandle,
		const char* EntryPoint = "__miss__main");

	static OptixProgramGroupDesc MakeExceptionProgGroupDescription(
		OptixModule ModuleHandle,
		const char* EntryPoint = "__exception__main");

	static OptixProgramGroupDesc MakeCallableProgGroupDescription(
		OptixModule DirectCallableModuleHandle,
		OptixModule ContinuationCallableModuleHandle,
		const char* DCEntryPoint = "__direct_callable__main",
		const char* CCEntryPoint = "__continuation_callable__main");

	static OptixProgramGroupDesc MakeHitGroupProgGroupDescription(
		OptixModule ClosestHitModuleHandle,
		OptixModule AnyHitModuleHandle,
		OptixModule IntersectionModuleHandle,
		const char* CHEntryPoint = "__closesthit__main",
		const char* AHEntryPoint = "__anyhit__main",
		const char* IEntryPoint = "__intersection__main");

	constexpr auto GetHandle() { return Handle; }

	FCarlaOptiXProgramGroup() = default;
	FCarlaOptiXProgramGroup(
		FCarlaOptiXInstance& Instance,
		std::span<OptixProgramGroupDesc> Descriptions,
		const OptixProgramGroupOptions& Options);
	FCarlaOptiXProgramGroup(const FCarlaOptiXProgramGroup&) = delete;
	FCarlaOptiXProgramGroup& operator=(const FCarlaOptiXProgramGroup&) = delete;
	FCarlaOptiXProgramGroup(FCarlaOptiXProgramGroup&& Other);
	FCarlaOptiXProgramGroup& operator=(FCarlaOptiXProgramGroup&& Other);
	~FCarlaOptiXProgramGroup();

	void Destroy();
};