#pragma once
#include "OptiX_Common.h"
#include "OptiX_Module.h"
#include "OptiX_ProgramGroup.h"



class CARLAOPTIX_API FCarlaOptiXInstance;
class CARLAOPTIX_API FCarlaOptiXDeviceBuffer;
class CARLAOPTIX_API FCarlaOptiXShaderBindingTable;



class CARLAOPTIX_API FCarlaOptiXPipeline
{
	OptixPipeline Handle;

	static constexpr auto DefaultLinkOptions = OptixPipelineLinkOptions
	{
		.maxTraceDepth = 31
	};

public:

	FCarlaOptiXPipeline();
	FCarlaOptiXPipeline(
		FCarlaOptiXInstance& Instance,
		std::span<OptixProgramGroup> ProgGroups,
		const OptixPipelineCompileOptions& CompileOptions,
		const OptixPipelineLinkOptions& LinkOptions = DefaultLinkOptions);
	FCarlaOptiXPipeline(
		FCarlaOptiXInstance& Instance,
		std::span<FCarlaOptiXProgramGroup> ProgGroups,
		const OptixPipelineCompileOptions& CompileOptions,
		const OptixPipelineLinkOptions& LinkOptions = DefaultLinkOptions);
	FCarlaOptiXPipeline(const FCarlaOptiXPipeline&) = delete;
	FCarlaOptiXPipeline& operator=(const FCarlaOptiXPipeline&) = delete;
	FCarlaOptiXPipeline(FCarlaOptiXPipeline&& Other);
	FCarlaOptiXPipeline& operator=(FCarlaOptiXPipeline&& Other);
	~FCarlaOptiXPipeline();

	void Destroy();

	constexpr bool IsValid() const { return Handle != OptixPipeline(); }

	void Launch(
		FCarlaOptiXShaderBindingTable& SBT,
		FCarlaOptiXDeviceBuffer& ParamsBuffer,
		FUintVector3 WorkloadShape);

};