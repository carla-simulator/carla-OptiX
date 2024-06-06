#include "OptiX_Instance.h"
#ifdef CARLA_OPTIX_DEBUG_CHECKS
#include <atomic>
#endif

#ifndef CARLA_OPTIX_VERBOSE
constexpr bool CarlaOptiXVerboseDefault = false;
#else
constexpr bool CarlaOptiXVerboseDefault = true;
#endif

TAutoConsoleVariable<int32> CarlaOptiXVerbose(
	TEXT("CARLA.OptiX.Verbose"),
	(int32)CarlaOptiXVerboseDefault,
	TEXT("Whether to enable extra error messages from CarlaOptiX."));



static void* OptixLibraryHandle = nullptr;

#ifdef CARLA_OPTIX_DEBUG_CHECKS
static std::atomic<FCarlaOptiXInstance*> GlobalOptiXInstance = nullptr;
#else
static FCarlaOptiXInstance* GlobalOptiXInstance = nullptr;
#endif



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
	CheckOptiXResult(optixInitWithHandle(&OptixLibraryHandle));
}

void FCarlaOptiXInstance::DestroyGlobalContext()
{
	auto result = optixUninitWithHandle(OptixLibraryHandle);
	CheckOptiXResult(result);
}

FCarlaOptiXInstance* FCarlaOptiXInstance::GetGlobalInstance()
{
#ifdef CARLA_OPTIX_DEBUG_CHECKS
	return GlobalOptiXInstance.load(std::memory_order::acquire);
#else
	return GlobalOptiXInstance;
#endif
}

FCarlaOptiXInstance& FCarlaOptiXInstance::GetGlobalInstanceChecked()
{
	auto Ptr = GetGlobalInstance();
	check(Ptr != nullptr);
	return *Ptr;
}

FCarlaOptiXInstance::FCarlaOptiXInstance() :
	CUDAContext(),
	OptixContext()
{
}

FCarlaOptiXInstance::FCarlaOptiXInstance(
	const FCarlaOptiXInstanceOptions& OptiXOptions) :
	CUDAContext(),
	CUDADevice(),
	OptixContext()
{
	check(!IsValid());
	CARLA_OPTIX_LOG(TEXT("Creating Instance."));
	unsigned CudaContextFlags = 0;
	CARLA_OPTIX_LOG(TEXT("Searching for compute devices."));
	CheckCUDAResult(cuDeviceGet(&CUDADevice, OptiXOptions.CUDADeviceIndex));
	CARLA_OPTIX_LOG(TEXT("Creating CUDA context."));
	CheckCUDAResult(cuCtxCreate(&CUDAContext, CudaContextFlags, CUDADevice));
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
	CARLA_OPTIX_LOG(TEXT("Creating OptiX context."));
	CheckOptiXResult(optixDeviceContextCreate(
		CUDAContext,
		&OptixOptions,
		&OptixContext));
}

FCarlaOptiXInstance::FCarlaOptiXInstance(FCarlaOptiXInstance&& Other) :
	CUDAContext(Other.CUDAContext),
	CUDADevice(Other.CUDADevice),
	OptixContext(Other.OptixContext)
{
	Other.CUDAContext = CUcontext();
	Other.CUDADevice = CUdevice();
	Other.OptixContext = OptixDeviceContext();
}

FCarlaOptiXInstance& FCarlaOptiXInstance::operator=(FCarlaOptiXInstance&& Other)
{
	this->~FCarlaOptiXInstance();
	new (this) FCarlaOptiXInstance(std::move(Other));
	return *this;
}

FCarlaOptiXInstance::~FCarlaOptiXInstance()
{
	if (!IsValid())
		Destroy();
}

void FCarlaOptiXInstance::Destroy()
{
	CARLA_OPTIX_LOG(TEXT("Destroying Instance."));
	if (OptixContext != OptixDeviceContext())
	{
		CARLA_OPTIX_LOG(TEXT("Destroying OptiX context."));
		optixDeviceContextDestroy(OptixContext);
		OptixContext = OptixDeviceContext();
	}
}

bool FCarlaOptiXInstance::IsValid() const
{
	return
		CUDAContext != CUcontext() &&
		CUDADevice != CUdevice() &&
		OptixContext != OptixDeviceContext();
}

void FCarlaOptiXInstance::SetAsGlobalInstance()
{
	GlobalOptiXInstance = this;
}



ACarlaOptiXInstance::ACarlaOptiXInstance(
	const FObjectInitializer& Initializer) :
	Super(Initializer)
{
}

void ACarlaOptiXInstance::BeginPlay()
{
	FCarlaOptiXInstanceOptions Options = {};
	Implementation = FCarlaOptiXInstance(Options);
	Implementation.SetAsGlobalInstance();
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
		Error,
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
		Error,
		TEXT("\n %s (%u, %u): OptiX error 0x%llx (%s) encountered: %s\n"),
		*FString(loc.file_name()),
		loc.line(),
		loc.column(),
		(unsigned)ec,
		*FString(name ? name : "<invalid CUresult>"),
		*FString(message ? message : "<N/A>"));
	check(false);
}

bool IsCarlaOptiXVerboseLoggingEnabled()
{
	return (bool)CarlaOptiXVerbose.GetValueOnAnyThread();
}
