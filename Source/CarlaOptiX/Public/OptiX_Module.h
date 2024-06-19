#pragma once
#include "OptiX_Common.h"



class CARLAOPTIX_API FCarlaOptiXInstance;



class CARLAOPTIX_API FCarlaOptiXKernelModule
{
	OptixModule Module;
public:

	FCarlaOptiXKernelModule();
	FCarlaOptiXKernelModule(
		FCarlaOptiXInstance& Instance,
		std::span<const char> Source,
		const OptixModuleCompileOptions& ModuleOptions,
		const OptixPipelineCompileOptions& PipelineOptions);
	FCarlaOptiXKernelModule(const FCarlaOptiXKernelModule&) = delete;
	FCarlaOptiXKernelModule& operator=(const FCarlaOptiXKernelModule&) = delete;
	FCarlaOptiXKernelModule(FCarlaOptiXKernelModule&&);
	FCarlaOptiXKernelModule& operator=(FCarlaOptiXKernelModule&&);
	~FCarlaOptiXKernelModule();

	void Destroy();

	constexpr bool IsValid() const { return Module != OptixModule(); }
	constexpr auto GetHandle() const { return Module; }

};
