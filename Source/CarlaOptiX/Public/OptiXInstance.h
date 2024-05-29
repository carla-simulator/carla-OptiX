#pragma once
#include <optix.h>



class CARLAOPTIX_API FCarlaOptiXInstance
{
  CUcontext cuda_context;
  OptixDeviceContext optix_context;

public:

  static void InitGlobalContext();
  static void DestroyGlobalContext();

  FCarlaOptiXInstance();
  FCarlaOptiXInstance(
    const OptixDeviceContextOptions& optix_options);
  FCarlaOptiXInstance(const FCarlaOptiXInstance&) = delete;
  FCarlaOptiXInstance& operator=(const FCarlaOptiXInstance&) = delete;
  ~FCarlaOptiXInstance();

};
