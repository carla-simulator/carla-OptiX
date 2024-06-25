#include "OptiX_ProgramGroup.h"
#include "OptiX_Instance.h"
#include "OptiX_Module.h"

extern thread_local char LogBuffer[CARLA_OPTIX_LOG_BUFFER_SIZE];

OptixProgramGroupDesc FCarlaOptiXProgramGroup::MakeRayGenProgGroupDescription(
	OptixModule ModuleHandle,
	const char* EntryPoint)
{
	OptixProgramGroupDesc Result = { };
	Result.kind = OPTIX_PROGRAM_GROUP_KIND_RAYGEN;
	Result.raygen.module = ModuleHandle;
	Result.raygen.entryFunctionName = EntryPoint;
	return Result;
}

OptixProgramGroupDesc FCarlaOptiXProgramGroup::MakeMissProgGroupDescription(
	OptixModule ModuleHandle,
	const char* EntryPoint)
{
	OptixProgramGroupDesc Result = { };
	Result.kind = OPTIX_PROGRAM_GROUP_KIND_MISS;
	Result.miss.module = ModuleHandle;
	Result.miss.entryFunctionName = EntryPoint;
	return Result;
}

OptixProgramGroupDesc FCarlaOptiXProgramGroup::MakeExceptionProgGroupDescription(
	OptixModule ModuleHandle,
	const char* EntryPoint)
{
	OptixProgramGroupDesc Result = { };
	Result.kind = OPTIX_PROGRAM_GROUP_KIND_EXCEPTION;
	Result.exception.module = ModuleHandle;
	Result.exception.entryFunctionName = EntryPoint;
	return Result;
}

OptixProgramGroupDesc FCarlaOptiXProgramGroup::MakeCallableProgGroupDescription(
	OptixModule DirectCallableModuleHandle,
	OptixModule ContinuationCallableModuleHandle,
	const char* DCEntryPoint,
	const char* CCEntryPoint)
{
	OptixProgramGroupDesc Result = { };
	Result.kind = OPTIX_PROGRAM_GROUP_KIND_CALLABLES;
	Result.callables.moduleDC = DirectCallableModuleHandle;
	Result.callables.moduleCC = ContinuationCallableModuleHandle;
	Result.callables.entryFunctionNameDC = DCEntryPoint;
	Result.callables.entryFunctionNameCC = CCEntryPoint;
	return Result;
}

OptixProgramGroupDesc FCarlaOptiXProgramGroup::MakeHitGroupProgGroupDescription(
	OptixModule ClosestHitModuleHandle,
	OptixModule AnyHitModuleHandle,
	OptixModule IntersectionModuleHandle,
	const char* CHEntryPoint,
	const char* AHEntryPoint,
	const char* IEntryPoint)
{
	OptixProgramGroupDesc Result = { };
	Result.kind = OPTIX_PROGRAM_GROUP_KIND_HITGROUP;
	Result.hitgroup.moduleCH = ClosestHitModuleHandle;
	Result.hitgroup.moduleAH = AnyHitModuleHandle;
	Result.hitgroup.moduleIS = IntersectionModuleHandle;
	Result.hitgroup.entryFunctionNameCH = CHEntryPoint;
	Result.hitgroup.entryFunctionNameAH = AHEntryPoint;
	Result.hitgroup.entryFunctionNameIS = IEntryPoint;
	return Result;
}

FCarlaOptiXProgramGroup::FCarlaOptiXProgramGroup() :
	Handle()
{
}

FCarlaOptiXProgramGroup::FCarlaOptiXProgramGroup(
	FCarlaOptiXInstance& Instance,
	std::span<OptixProgramGroupDesc> Descriptions,
	const OptixProgramGroupOptions& Options)
{
	size_t LogBufferSize = CARLA_OPTIX_LOG_BUFFER_SIZE;
	CheckOptiXResult(optixProgramGroupCreate(
		Instance.GetOptixDeviceContext(),
		Descriptions.data(),
		Descriptions.size(),
		&Options,
		LogBuffer,
		&LogBufferSize,
		&Handle));
}

FCarlaOptiXProgramGroup::FCarlaOptiXProgramGroup(FCarlaOptiXProgramGroup&& Other) :
	Handle(Other.Handle)
{
	Other.Handle = OptixProgramGroup();
}

FCarlaOptiXProgramGroup& FCarlaOptiXProgramGroup::operator=(FCarlaOptiXProgramGroup&& Other)
{
	this->~FCarlaOptiXProgramGroup();
	new (this) FCarlaOptiXProgramGroup(std::move(Other));
	return *this;
}

FCarlaOptiXProgramGroup::~FCarlaOptiXProgramGroup()
{
	if (Handle != OptixProgramGroup())
		Destroy();
}

void FCarlaOptiXProgramGroup::Destroy()
{
	check(Handle != OptixProgramGroup());
	optixProgramGroupDestroy(Handle);
	Handle = OptixProgramGroup();
}
