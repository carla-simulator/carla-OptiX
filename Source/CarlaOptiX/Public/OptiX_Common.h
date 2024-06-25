#pragma once
#include <optix.h>
#include <optix_denoiser_tiling.h>
#include <optix_function_table_definition.h>
#include <optix_function_table.h>
// #include <optix_micromap.h> // This requires defining vector types.
#include <optix_stack_size.h>
#include <optix_stubs.h>
#include <optix_types.h>

#include <source_location>



constexpr size_t CARLA_OPTIX_LOG_BUFFER_SIZE = 4096;



template <typename T>
using DevicePtr = CUdeviceptr;



enum class EModuleStageKind : uint8_t
{
	AnyHit,
	ClosestHit,
	Miss,
	Intersection,
	RayGen,
	Exception,
	Callable // @This may be incorrect.
};



void CheckCUDAResult(
	CUresult ec,
	std::source_location loc = std::source_location::current());

void CheckOptiXResult(
	OptixResult ec,
	std::source_location loc = std::source_location::current());

bool IsCarlaOptiXVerboseLoggingEnabled();



constexpr auto RoundToAlignment(size_t x, size_t a)
{
	--a;
	return ((x + a) & ~(size_t)a);
}

constexpr size_t ToWordCount(size_t x)
{
	return RoundToAlignment(x, 4) >> 2;
}

template <typename T>
constexpr size_t GetWordCountOf = ToWordCount(sizeof(T));



#define CARLA_OPTIX_LOG(...) \
	UE_LOG(LogCarlaOptiX, Log, __VA_ARGS__)



#define CARLA_OPTIX_WARNING(...) \
	UE_LOG(LogCarlaOptiX, Warning, __VA_ARGS__)



#define CARLA_OPTIX_ERROR(...) \
	UE_LOG(LogCarlaOptiX, Error, __VA_ARGS__)



#ifdef CARLA_OPTIX_VERBOSE
#define CARLA_OPTIX_LOG_VERBOSE(...) \
	if (IsCarlaOptiXVerboseLoggingEnabled()) \
		CARLA_OPTIX_LOG(__VA_ARGS__)
#else
#define CARLA_OPTIX_LOG_VERBOSE(...)
#endif