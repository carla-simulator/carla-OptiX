#include "OptiXInstance.h"
#include <mutex>

static void* optix_library_handle = nullptr;

void FCarlaOptiXInstance::InitGlobalContext()
{
  auto cuda_result = cuInit(0);
  check(cuda_result == CUDA_SUCCESS);
  auto optix_result = optixInitWithHandle(&optix_library_handle);
  check(optix_result == OPTIX_SUCCESS);
}

void FCarlaOptiXInstance::DestroyGlobalContext()
{
  auto result = optixUninitWithHandle(optix_library_handle);
  check(result == OPTIX_SUCCESS);
}

FCarlaOptiXInstance::FCarlaOptiXInstance() :
  cuda_context(),
  optix_context()
{
}

FCarlaOptiXInstance::FCarlaOptiXInstance(
  const OptixDeviceContextOptions& optix_options) :
  cuda_context(0),
  optix_context()
{
  optixDeviceContextCreate(
    cuda_context,
    &optix_options,
    &optix_context);
}

FCarlaOptiXInstance::~FCarlaOptiXInstance()
{
  if (optix_context != OptixDeviceContext())
  {
    optixDeviceContextDestroy(optix_context);
    optix_context = OptixDeviceContext();
  }
}

