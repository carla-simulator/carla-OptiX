#pragma once
#include "CoreMinimal.h"
#include "OptiXHeaders.h"
#include "SceneCaptureComponent_NV_OptiX.generated.h"



class FCarlaOptiXInstance;



UCLASS()
class CARLAOPTIX_API ASceneCaptureComponentNVOptiX :
	public AActor
{
	GENERATED_BODY()

	FCarlaOptiXInstance* OptixInstance;

public:

	ASceneCaptureComponentNVOptiX(
		const FObjectInitializer& ObjectInitializer);

};
