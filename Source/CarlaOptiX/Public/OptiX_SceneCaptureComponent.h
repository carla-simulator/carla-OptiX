#pragma once
#include "CoreMinimal.h"
#include "OptiX_Common.h"
#include "OptiX_SceneCaptureComponent.generated.h"



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
