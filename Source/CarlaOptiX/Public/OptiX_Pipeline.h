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
public:

	FCarlaOptiXPipeline() = default;
	FCarlaOptiXPipeline(
		FCarlaOptiXInstance& Instance,
		std::span<OptixProgramGroup> ProgGroups,
		const OptixPipelineCompileOptions& CompileOptions,
		const OptixPipelineLinkOptions& LinkOptions);
	FCarlaOptiXPipeline(
		FCarlaOptiXInstance& Instance,
		std::span<FCarlaOptiXProgramGroup> ProgGroups,
		const OptixPipelineCompileOptions& CompileOptions,
		const OptixPipelineLinkOptions& LinkOptions);
	FCarlaOptiXPipeline(const FCarlaOptiXPipeline&) = delete;
	FCarlaOptiXPipeline& operator=(const FCarlaOptiXPipeline&) = delete;
	FCarlaOptiXPipeline(FCarlaOptiXPipeline&& Other);
	FCarlaOptiXPipeline& operator=(FCarlaOptiXPipeline&& Other);
	~FCarlaOptiXPipeline();

	template <EModuleStageKind... EnabledKinds>
	static auto CreateFromTraits(
		FCarlaOptiXInstance& Instance,
		const TCHAR* PTXPath,
		const OptixPipelineCompileOptions& CompileOptions,
		unsigned MaxTraceDepth = 31)
	{
		std::vector<OptixProgramGroupDesc> ProgGroupDescs;

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

		FCarlaOptiXKernelModule Module(
			Instance,
			std::span((const char*)OptiXByteCode.GetData(), OptiXByteCode.Num()),
			ModuleOptions,
			CompileOptions);

		bool HitGroupFound = false;
		constexpr EModuleStageKind StageKinds[] = { EnabledKinds... };
		for (auto StageKind : StageKinds)
		{
			switch (StageKind)
			{
			case EModuleStageKind::AnyHit:
			case EModuleStageKind::ClosestHit:
			case EModuleStageKind::Intersection:
				if (!HitGroupFound)
				{
					ProgGroupDescs.push_back(FCarlaOptiXProgramGroup::MakeHitGroupProgGroupDescription(
						Module.GetHandle(),
						Module.GetHandle(),
						Module.GetHandle()));
					HitGroupFound = true;
				}
				break;
			case EModuleStageKind::Miss:
				ProgGroupDescs.push_back(FCarlaOptiXProgramGroup::MakeMissProgGroupDescription(
					Module.GetHandle()));
				break;
			case EModuleStageKind::RayGen:
				ProgGroupDescs.push_back(FCarlaOptiXProgramGroup::MakeRayGenProgGroupDescription(
					Module.GetHandle()));
				break;
			case EModuleStageKind::Exception:
				ProgGroupDescs.push_back(FCarlaOptiXProgramGroup::MakeExceptionProgGroupDescription(
					Module.GetHandle()));
				break;
			case EModuleStageKind::Callable:
				ProgGroupDescs.push_back(FCarlaOptiXProgramGroup::MakeCallableProgGroupDescription(
					Module.GetHandle(),
					Module.GetHandle()));
				break;
			default:
				break;
			}
		}

		OptixProgramGroupOptions ProgGroupOptions = { };
		FCarlaOptiXProgramGroup ProgramGroup(
			Instance,
			ProgGroupDescs,
			ProgGroupOptions);

		OptixPipelineLinkOptions LinkOptions = {};
		LinkOptions.maxTraceDepth = MaxTraceDepth;

		FCarlaOptiXPipeline Pipeline(
			Instance,
			std::span(&ProgramGroup, 1),
			CompileOptions,
			LinkOptions);

		return std::make_tuple(
			std::move(Pipeline),
			std::move(Module));
	}

	void Destroy();

	constexpr bool IsValid() const { return Handle != OptixPipeline(); }

	void Launch(
		FCarlaOptiXShaderBindingTable& SBT,
		FCarlaOptiXDeviceBuffer& ParamsBuffer,
		FUintVector3 WorkloadShape);

};