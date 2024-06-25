#pragma once
#include "OptiX_Common.h"
#include "OptiX_Module.h"
#include <span>



class CARLAOPTIX_API FCarlaOptiXInstance;



class CARLAOPTIX_API FCarlaOptiXProgramGroup
{
	OptixProgramGroup Handle;

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

	FCarlaOptiXProgramGroup();
	FCarlaOptiXProgramGroup(
		FCarlaOptiXInstance& Instance,
		std::span<OptixProgramGroupDesc> Descriptions,
		const OptixProgramGroupOptions& Options);
	FCarlaOptiXProgramGroup(const FCarlaOptiXProgramGroup&) = delete;
	FCarlaOptiXProgramGroup& operator=(const FCarlaOptiXProgramGroup&) = delete;
	FCarlaOptiXProgramGroup(FCarlaOptiXProgramGroup&& Other);
	FCarlaOptiXProgramGroup& operator=(FCarlaOptiXProgramGroup&& Other);
	~FCarlaOptiXProgramGroup();

	template <EModuleStageKind... EnabledKinds>
	static auto CreateFromSingleModule(
		FCarlaOptiXInstance& Instance,
		FCarlaOptiXKernelModule& Module)
	{
		constexpr EModuleStageKind StageKinds[] = { EnabledKinds... };
		auto ModuleHandle = Module.GetHandle();
		std::vector<OptixProgramGroupDesc> ProgGroupDescs;
		bool HitGroupFound = false;
		for (auto StageKind : StageKinds)
		{
			switch (StageKind)
			{
			case EModuleStageKind::AnyHit:
			case EModuleStageKind::ClosestHit:
			case EModuleStageKind::Intersection:
				if (!HitGroupFound)
				{
					HitGroupFound = true;
					ProgGroupDescs.push_back(FCarlaOptiXProgramGroup::MakeHitGroupProgGroupDescription(
						ModuleHandle,
						ModuleHandle,
						ModuleHandle));
				}
				break;
			case EModuleStageKind::Miss:
				ProgGroupDescs.push_back(FCarlaOptiXProgramGroup::MakeMissProgGroupDescription(
					ModuleHandle));
				break;
			case EModuleStageKind::RayGen:
				ProgGroupDescs.push_back(FCarlaOptiXProgramGroup::MakeRayGenProgGroupDescription(
					ModuleHandle));
				break;
			case EModuleStageKind::Exception:
				ProgGroupDescs.push_back(FCarlaOptiXProgramGroup::MakeExceptionProgGroupDescription(
					ModuleHandle));
				break;
			case EModuleStageKind::Callable:
				ProgGroupDescs.push_back(FCarlaOptiXProgramGroup::MakeCallableProgGroupDescription(
					ModuleHandle,
					ModuleHandle));
				break;
			default:
				break;
			}
		}
		OptixProgramGroupOptions ProgGroupOptions = { };
		return FCarlaOptiXProgramGroup(
			Instance,
			ProgGroupDescs,
			ProgGroupOptions);
	}

	void Destroy();
};