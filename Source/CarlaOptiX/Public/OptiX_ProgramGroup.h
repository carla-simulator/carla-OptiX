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
		const char* EntryPoint = "RayGenMain");

	static OptixProgramGroupDesc MakeMissProgGroupDescription(
		OptixModule ModuleHandle,
		const char* EntryPoint = "MissMain");

	static OptixProgramGroupDesc MakeExceptionProgGroupDescription(
		OptixModule ModuleHandle,
		const char* EntryPoint = "ExceptionMain");

	static OptixProgramGroupDesc MakeCallableProgGroupDescription(
		OptixModule DirectCallableModuleHandle,
		OptixModule ContinuationCallableModuleHandle,
		const char* DCEntryPoint = "DirectCallableMain",
		const char* CCEntryPoint = "ContinuationCallableMain");

	static OptixProgramGroupDesc MakeHitGroupProgGroupDescription(
		OptixModule ClosestHitModuleHandle,
		OptixModule AnyHitModuleHandle,
		OptixModule IntersectionModuleHandle,
		const char* CHEntryPoint = "ClosestHitMain",
		const char* AHEntryPoint = "AnyHitMain",
		const char* IEntryPoint = "IntersectionMain");

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

	template <typename T>
	static FCarlaOptiXProgramGroup CreateFromTraitsType(
		FCarlaOptiXInstance& Instance,
		const OptixProgramGroupOptions& Options,
		const OptixPipelineCompileOptions& PipelineCompileOptions,
		std::vector<FCarlaOptiXKernelModule>& OutModules)
	{
		const std::string_view RayGen = T::RayGen;
		const std::string_view AnyHit = T::AnyHit;
		const std::string_view ClosestHit = T::ClosestHit;
		const std::string_view Intersection = T::Intersection;
		const std::string_view Miss = T::Miss;

		std::vector<OptixProgramGroupDesc> ProgDescs;
		ProgDescs.reserve(5);

		auto AH = OptixModule();
		auto CH = OptixModule();
		auto IS = OptixModule();
		size_t HitGroupHandleCount = 0;

		auto AddModule = [&](std::string_view Source)
		{
			check(!Source.empty());
			OptixModuleCompileOptions Options; // @TODO
			OutModules.push_back(FCarlaOptiXKernelModule(
				Instance,
				std::span(Source.data(), Source.size()),
				Options,
				PipelineCompileOptions));
			return OutModules.back().GetHandle();
		};

		if (!RayGen.empty())
			ProgDescs.push_back(MakeRayGenProgGroupDescription(AddModule(RayGen)));

		if (!Miss.empty())
			ProgDescs.push_back(MakeMissProgGroupDescription(AddModule(Miss)));

		if (!AnyHit.empty())
		{
			AH = AddModule(AnyHit);
			++HitGroupHandleCount;
		}

		if (!ClosestHit.empty())
		{
			CH = AddModule(ClosestHit);
			++HitGroupHandleCount;
		}

		if (!Intersection.empty())
		{
			IS = AddModule(Intersection);
			++HitGroupHandleCount;
		}

		if (HitGroupHandleCount != 0)
			ProgDescs.push_back(MakeHitGroupProgGroupDescription(CH, AH, IS));

		return FCarlaOptiXProgramGroup(Instance, ProgDescs, Options);
	}

};