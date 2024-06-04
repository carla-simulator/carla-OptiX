#include "OptiXInstance.h"
#include <mutex>

TAutoConsoleVariable<int32> CarlaOptiXVerbose(
	TEXT("CARLA.OptiX.Verbose"),
	1,
	TEXT("Whether to enable extra error message from CarlaOptiX."));

static void* optix_library_handle = nullptr;

void FCarlaOptiXInstance::OptixCallback(
	unsigned int level,
	const char* tag,
	const char* message,
	void* self_ptr)
{
	FCarlaOptiXInstance& self =
		*reinterpret_cast<FCarlaOptiXInstance*>(self_ptr);
	UE_LOG(
		LogCarlaOptiX,
		Log,
		TEXT("Optix-%s: %s"),
		*FString(tag),
		*FString(message));
}

void FCarlaOptiXInstance::InitGlobalContext()
{
	CheckCUDAResult(cuInit(0));
	CheckOptiXResult(optixInitWithHandle(&optix_library_handle));

	FOptixHostArray<int> temp(4096);
	FOptixDeviceArray<int> temp2(4096);
}

void FCarlaOptiXInstance::DestroyGlobalContext()
{
	auto result = optixUninitWithHandle(optix_library_handle);
	CheckOptiXResult(result);
}

FCarlaOptiXInstance::FCarlaOptiXInstance() :
	CUDAContext(),
	OptixContext()
{
}

FCarlaOptiXInstance::FCarlaOptiXInstance(
	const FCarlaOptiXInstanceOptions& Options) :
	CUDAContext(),
	CUDADevice(),
	OptixContext()
{
	unsigned CudaContextFlags = 0;
	cuDeviceGet(&CUDADevice, Options.CUDADeviceIndex);
	cuCtxCreate(&CUDAContext, CudaContextFlags, CUDADevice);
	OptixDeviceContextOptions OptixOptions = {};
	OptixOptions.logCallbackFunction = OptixCallback;
	OptixOptions.logCallbackData = this;
	if (CarlaOptiXVerbose.GetValueOnAnyThread() == 1)
	{
		OptixOptions.logCallbackLevel = 4;
		OptixOptions.validationMode = OPTIX_DEVICE_CONTEXT_VALIDATION_MODE_ALL;
	}
	else
	{
		OptixOptions.logCallbackLevel = 3;
		OptixOptions.validationMode = OPTIX_DEVICE_CONTEXT_VALIDATION_MODE_OFF;
	}
	optixDeviceContextCreate(
		CUDAContext,
		&OptixOptions,
		&OptixContext);
}

FCarlaOptiXInstance::~FCarlaOptiXInstance()
{
	if (OptixContext != OptixDeviceContext())
	{
		optixDeviceContextDestroy(OptixContext);
		OptixContext = OptixDeviceContext();
	}
}

void CheckCUDAResult(
	CUresult ec,
	std::source_location loc)
{
	check(ec == CUDA_SUCCESS);
	if (ec == CUDA_SUCCESS)
		return;
	const char* name = nullptr;
	const char* message = nullptr;
	(void)cuGetErrorName(ec, &name);
	(void)cuGetErrorString(ec, &message);
	UE_LOG(
		LogCarlaOptiX,
		Log,
		TEXT("\n %s (%u, %u): CUDA error 0x%llx (%s) encountered: %s\n"),
		*FString(loc.file_name()),
		loc.line(),
		loc.column(),
		(unsigned)ec,
		*FString(name ? name : "<invalid CUresult>"),
		*FString(message ? message : "<N/A>"));
	check(false);
}

void CheckOptiXResult(
	OptixResult ec,
	std::source_location loc)
{
	check(ec == OPTIX_SUCCESS);
	if (ec == OPTIX_SUCCESS)
		return;
	auto name = optixGetErrorName(ec);
	auto message = optixGetErrorString(ec);
	UE_LOG(
		LogCarlaOptiX,
		Log,
		TEXT("\n %s (%u, %u): OptiX error 0x%llx (%s) encountered: %s\n"),
		*FString(loc.file_name()),
		loc.line(),
		loc.column(),
		(unsigned)ec,
		*FString(name ? name : "<invalid CUresult>"),
		*FString(message ? message : "<N/A>"));
	check(false);
}