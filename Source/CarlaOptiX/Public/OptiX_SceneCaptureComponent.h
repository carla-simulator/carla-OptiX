#pragma once
#include "CoreMinimal.h"
#include "OptiX_Common.h"
#include "OptiX_SceneCaptureComponent.generated.h"



class FCarlaOptiXScene;



UCLASS()
class CARLAOPTIX_API ASceneCaptureComponentNVOptiX :
	public AActor
{
	GENERATED_BODY()

	FCarlaOptiXScene* Scene;

public:

	ASceneCaptureComponentNVOptiX(
		const FObjectInitializer& ObjectInitializer);

	void BeginPlay() override;

	void EndPlay(
		const EEndPlayReason::Type EndPlayReason) override;

	void RenderScene();

};
