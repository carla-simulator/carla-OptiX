#include "OptiX_Module.h"
#include "OptiX_Instance.h"

extern thread_local char LogBuffer[CARLA_OPTIX_LOG_BUFFER_SIZE];

FCarlaOptiXKernelModule::FCarlaOptiXKernelModule()
{
}

FCarlaOptiXKernelModule::FCarlaOptiXKernelModule(
	FCarlaOptiXInstance& Instance,
	std::span<const char> Source,
	const OptixModuleCompileOptions& ModuleOptions,
	const OptixPipelineCompileOptions& PipelineOptions)
{
	size_t LogBufferSize = CARLA_OPTIX_LOG_BUFFER_SIZE;
	CheckOptiXResult(optixModuleCreate(
		Instance.GetOptixDeviceContext(),
		&ModuleOptions,
		&PipelineOptions,
		Source.data(),
		Source.size(),
		LogBuffer,
		&LogBufferSize,
		&Module));
}

FCarlaOptiXKernelModule::FCarlaOptiXKernelModule(FCarlaOptiXKernelModule&& Other) :
	Module(Other.Module)
{
	Other.Module = OptixModule();
}

FCarlaOptiXKernelModule& FCarlaOptiXKernelModule::operator=(FCarlaOptiXKernelModule&& Other)
{
	this->~FCarlaOptiXKernelModule();
	new (this) FCarlaOptiXKernelModule(std::move(Other));
	return *this;
}

FCarlaOptiXKernelModule::~FCarlaOptiXKernelModule()
{
	if (Module != OptixModule())
	{
		optixModuleDestroy(Module);
		Module = OptixModule();
	}
}

