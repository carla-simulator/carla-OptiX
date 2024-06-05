#pragma once
#include "OptiXHeaders.h"

#include <Instance.generated.h>



struct FCarlaOptiXInstanceOptions
{
	size_t CUDADeviceIndex = 0;
};



class CARLAOPTIX_API FCarlaOptiXInstance
{
	CUcontext CUDAContext;
	CUdevice CUDADevice;
	OptixDeviceContext OptixContext;

	static void OptixCallback(
		unsigned int level,
		const char* tag,
		const char* message,
		void* self);

public:

	static void InitGlobalContext();
	static void DestroyGlobalContext();

	FCarlaOptiXInstance();
	FCarlaOptiXInstance(
		const FCarlaOptiXInstanceOptions& optix_options);
	FCarlaOptiXInstance(const FCarlaOptiXInstance&) = delete;
	FCarlaOptiXInstance& operator=(const FCarlaOptiXInstance&) = delete;
	~FCarlaOptiXInstance();

	constexpr auto GetCUDAContext() { return CUDAContext; }
	constexpr auto GetOptixDeviceContext() { return OptixContext; }

};



UCLASS()
class CARLAOPTIX_API ACarlaOptiXInstance :
	public AActor,
	public FCarlaOptiXInstance
{
	GENERATED_BODY()
public:

	ACarlaOptiXInstance(const FObjectInitializer&);

};
