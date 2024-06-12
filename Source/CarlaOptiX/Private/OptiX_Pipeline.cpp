#include "OptiX_Pipeline.h"
#include "OptiX_Instance.h"
#include "OptiX_Buffer.h"
#include "OptiX_ShaderBindingTable.h"

extern thread_local char LogBuffer[CARLA_OPTIX_LOG_BUFFER_SIZE];

FCarlaOptiXPipeline::FCarlaOptiXPipeline(
	FCarlaOptiXInstance& Instance,
	std::span<OptixProgramGroup> ProgGroups,
	const OptixPipelineCompileOptions& CompileOptions,
	const OptixPipelineLinkOptions& LinkOptions)
{
	size_t LogBufferSize = CARLA_OPTIX_LOG_BUFFER_SIZE;
	CheckOptiXResult(optixPipelineCreate(
		Instance.GetOptixDeviceContext(),
		&CompileOptions,
		&LinkOptions,
		ProgGroups.data(),
		ProgGroups.size(),
		LogBuffer,
		&LogBufferSize,
		&Handle));
}

FCarlaOptiXPipeline::FCarlaOptiXPipeline(FCarlaOptiXPipeline&& Other) :
	Handle(Other.Handle)
{
	Other.Handle = OptixPipeline();
}

FCarlaOptiXPipeline& FCarlaOptiXPipeline::operator=(FCarlaOptiXPipeline&& Other)
{
	this->~FCarlaOptiXPipeline();
	new (this) FCarlaOptiXPipeline(std::move(Other));
	return *this;
}

FCarlaOptiXPipeline::~FCarlaOptiXPipeline()
{
	if (Handle != OptixPipeline())
		Destroy();
}

void FCarlaOptiXPipeline::Destroy()
{
	check(Handle != OptixPipeline());
	CheckOptiXResult(optixPipelineDestroy(Handle));
	Handle = OptixPipeline();
}

void FCarlaOptiXPipeline::Launch(
	FCarlaOptiXShaderBindingTable& SBT,
	FCarlaOptiXDeviceBuffer& ParamsBuffer,
	FUintVector3 WorkloadShape)
{
	CheckOptiXResult(optixLaunch(
		Handle,
		0,
		ParamsBuffer.GetDeviceAddress(),
		ParamsBuffer.GetSizeBytes(),
		&SBT.GetSBT(),
		WorkloadShape.X,
		WorkloadShape.Y,
		WorkloadShape.Z));
}
