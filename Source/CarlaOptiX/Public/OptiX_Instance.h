#pragma once
#include "OptiX_Common.h"

#include <OptiX_Instance.generated.h>



USTRUCT(BlueprintType)
struct FCarlaOptiXInstanceOptions
{
	GENERATED_BODY()

	UPROPERTY()
	int32 CUDADeviceIndex = 0;

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

	static FCarlaOptiXInstance* GetGlobalInstance();
	static FCarlaOptiXInstance& GetGlobalInstanceChecked();

	FCarlaOptiXInstance();
	FCarlaOptiXInstance(
		const FCarlaOptiXInstanceOptions& OptiXOptions);
	FCarlaOptiXInstance(const FCarlaOptiXInstance&) = delete;
	FCarlaOptiXInstance& operator=(const FCarlaOptiXInstance&) = delete;
	FCarlaOptiXInstance(FCarlaOptiXInstance&&);
	FCarlaOptiXInstance& operator=(FCarlaOptiXInstance&&);
	~FCarlaOptiXInstance();

	void Destroy();
	bool IsValid() const;

	void SetAsGlobalInstance();

	constexpr auto GetCUDAContext() { return CUDAContext; }
	constexpr auto GetOptixDeviceContext() { return OptixContext; }

};



UCLASS()
class CARLAOPTIX_API ACarlaOptiXInstance :
	public AActor
{
	GENERATED_BODY()
public:

	ACarlaOptiXInstance(
		const FObjectInitializer& Initializer);

	void BeginPlay() override;

private:

	FCarlaOptiXInstance Implementation;

};
