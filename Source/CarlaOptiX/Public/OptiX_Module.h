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

	static auto Load(
		FCarlaOptiXInstance& Instance,
		const TCHAR* PTXPath,
		const OptixPipelineCompileOptions& PipelineCompileOptions)
	{
		OptixModuleCompileOptions ModuleOptions = { };
		ModuleOptions.maxRegisterCount = OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT;
#ifdef CARLA_OPTIX_DEBUG
		ModuleOptions.optLevel = OPTIX_COMPILE_OPTIMIZATION_LEVEL_0;
		ModuleOptions.debugLevel = OPTIX_COMPILE_DEBUG_LEVEL_FULL;
#else
		ModuleOptions.optLevel = OPTIX_COMPILE_OPTIMIZATION_LEVEL_3;
		ModuleOptions.debugLevel = OPTIX_COMPILE_DEBUG_LEVEL_NONE;
#endif

		TArray64<uint8> OptiXByteCode;
		check(FFileHelper::LoadFileToArray(
			OptiXByteCode,
			PTXPath));

		return FCarlaOptiXKernelModule(
			Instance,
			std::span((const char*)OptiXByteCode.GetData(), OptiXByteCode.Num()),
			ModuleOptions,
			PipelineCompileOptions);
	}

};
