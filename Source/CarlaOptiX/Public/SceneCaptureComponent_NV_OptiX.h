#pragma once
#include "CoreMinimal.h"
#include "SceneCaptureComponent_NV_OptiX.generated.h"



class CARLAOPTIX_API FCarlaOptiXInstance;



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
