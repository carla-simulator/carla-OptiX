#include "OptiX_ProgramGroup.h"
#include "OptiX_Instance.h"
#include "OptiX_Module.h"

extern thread_local char LogBuffer[4096];

OptixProgramGroupDesc FCarlaOptiXProgramGroup::MakeRayGenProgGroupDescription(
	FCarlaOptiXKernelModule& Module,
	const char* EntryPoint)
{
	OptixProgramGroupDesc Result = { };
	Result.kind = OPTIX_PROGRAM_GROUP_KIND_RAYGEN;
	Result.raygen.module = Module.GetHandle();
	Result.raygen.entryFunctionName = EntryPoint;
	return Result;
}

OptixProgramGroupDesc FCarlaOptiXProgramGroup::MakeMissProgGroupDescription(
	FCarlaOptiXKernelModule& Module,
	const char* EntryPoint)
{
	OptixProgramGroupDesc Result = { };
	Result.kind = OPTIX_PROGRAM_GROUP_KIND_MISS;
	Result.miss.module = Module.GetHandle();
	Result.miss.entryFunctionName = EntryPoint;
	return Result;
}

OptixProgramGroupDesc FCarlaOptiXProgramGroup::MakeExceptionProgGroupDescription(
	FCarlaOptiXKernelModule& Module,
	const char* EntryPoint)
{
	OptixProgramGroupDesc Result = { };
	Result.kind = OPTIX_PROGRAM_GROUP_KIND_EXCEPTION;
	Result.exception.module = Module.GetHandle();
	Result.exception.entryFunctionName = EntryPoint;
	return Result;
}

OptixProgramGroupDesc FCarlaOptiXProgramGroup::MakeCallableProgGroupDescription(
	FCarlaOptiXKernelModule& DirectCallableModule,
	FCarlaOptiXKernelModule& ContinuationCallableModule,
	const char* DCEntryPoint,
	const char* CCEntryPoint)
{
	OptixProgramGroupDesc Result = { };
	Result.kind = OPTIX_PROGRAM_GROUP_KIND_EXCEPTION;
	Result.callables.moduleDC = DirectCallableModule.GetHandle();
	Result.callables.moduleCC = ContinuationCallableModule.GetHandle();
	Result.callables.entryFunctionNameDC = DCEntryPoint;
	Result.callables.entryFunctionNameCC = CCEntryPoint;
	return Result;
}

OptixProgramGroupDesc FCarlaOptiXProgramGroup::MakeHitGroupProgGroupDescription(
	FCarlaOptiXKernelModule& ClosestHitModule,
	FCarlaOptiXKernelModule& AnyHitModule,
	FCarlaOptiXKernelModule& IntersectionModule,
	const char* CHEntryPoint,
	const char* AHEntryPoint,
	const char* IEntryPoint)
{
	OptixProgramGroupDesc Result = { };
	Result.kind = OPTIX_PROGRAM_GROUP_KIND_EXCEPTION;
	Result.hitgroup.moduleCH = ClosestHitModule.GetHandle();
	Result.hitgroup.moduleAH = AnyHitModule.GetHandle();
	Result.hitgroup.moduleIS = IntersectionModule.GetHandle();
	Result.hitgroup.entryFunctionNameCH = CHEntryPoint;
	Result.hitgroup.entryFunctionNameAH = AHEntryPoint;
	Result.hitgroup.entryFunctionNameIS = IEntryPoint;
	return Result;
}

FCarlaOptiXProgramGroup::FCarlaOptiXProgramGroup(
	FCarlaOptiXInstance& Instance,
	std::span<OptixProgramGroupDesc> Descriptions,
	const OptixProgramGroupOptions& Options)
{
	size_t LogBufferSize = sizeof(LogBuffer);
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
