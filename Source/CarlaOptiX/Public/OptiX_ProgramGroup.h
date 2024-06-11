#pragma once
#include "OptiX_Common.h"
#include <span>



class CARLAOPTIX_API FCarlaOptiXInstance;
class CARLAOPTIX_API FCarlaOptiXKernelModule;



class CARLAOPTIX_API FCarlaOptiXProgramGroup
{
	OptixProgramGroup Handle = OptixProgramGroup();

public:

	static constexpr const char DefaultEntryPointName[] = "Main";

	static OptixProgramGroupDesc MakeRayGenProgGroupDescription(
		FCarlaOptiXKernelModule& Module,
		const char* EntryPoint = DefaultEntryPointName);
	static OptixProgramGroupDesc MakeMissProgGroupDescription(
		FCarlaOptiXKernelModule& Module,
		const char* EntryPoint = DefaultEntryPointName);
	static OptixProgramGroupDesc MakeExceptionProgGroupDescription(
		FCarlaOptiXKernelModule& Module,
		const char* EntryPoint = DefaultEntryPointName);
	static OptixProgramGroupDesc MakeCallableProgGroupDescription(
		FCarlaOptiXKernelModule& DirectCallableModule,
		FCarlaOptiXKernelModule& ContinuationCallableModule,
		const char* DCEntryPoint = DefaultEntryPointName,
		const char* CCEntryPoint = DefaultEntryPointName);
	static OptixProgramGroupDesc MakeHitGroupProgGroupDescription(
		FCarlaOptiXKernelModule& ClosestHitModule,
		FCarlaOptiXKernelModule& AnyHitModule,
		FCarlaOptiXKernelModule& IntersectionModule,
		const char* CHEntryPoint = DefaultEntryPointName,
		const char* AHEntryPoint = DefaultEntryPointName,
		const char* IEntryPoint = DefaultEntryPointName);

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

	constexpr auto GetHandle() { return Handle; }
};