#pragma once
#include "CoreMinimal.h"
#include <optix.h>
#include <optix_denoiser_tiling.h>
#include <optix_function_table_definition.h>
#include <optix_function_table.h>
// #include <optix_micromap.h> // This requires defining vector types.
#include <optix_stack_size.h>
#include <optix_stubs.h>
#include <optix_types.h>
#include "SceneCaptureComponent_NV_OptiX.generated.h"



UCLASS()
class CARLAOPTIX_API ASceneCaptureComponentNVOptiX :
	public AActor
{
	GENERATED_BODY()
public:

	ASceneCaptureComponentNVOptiX(
		const FObjectInitializer& ObjectInitializer);

};