#pragma once
#include "OptiX_Common.h"



class CARLAOPTIX_API FCarlaOptiXInstance;
class CARLAOPTIX_API FCarlaOptiXDeviceBuffer;
class CARLAOPTIX_API FCarlaOptiXShaderBindingTable;



class CARLAOPTIX_API FCarlaOptiXPipeline
{
	OptixPipeline Handle;
public:

	FCarlaOptiXPipeline() = default;
	FCarlaOptiXPipeline(
		FCarlaOptiXInstance& Instance,
		std::span<OptixProgramGroup> ProgGroups,
		const OptixPipelineCompileOptions& CompileOptions,
		const OptixPipelineLinkOptions& LinkOptions);
	FCarlaOptiXPipeline(const FCarlaOptiXPipeline&) = delete;
	FCarlaOptiXPipeline& operator=(const FCarlaOptiXPipeline&) = delete;
	FCarlaOptiXPipeline(FCarlaOptiXPipeline&& Other);
	FCarlaOptiXPipeline& operator=(FCarlaOptiXPipeline&& Other);
	~FCarlaOptiXPipeline();

	void Destroy();

	void Launch(
		FCarlaOptiXShaderBindingTable& SBT,
		FCarlaOptiXDeviceBuffer& ParamsBuffer,
		FUintVector3 WorkloadShape);

};