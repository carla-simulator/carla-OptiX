#pragma once
#include "CoreMinimal.h"
#include "OptiX_Common.h"
#include "OptiX_SceneCaptureComponent.generated.h"



class CARLAOPTIX_API FCarlaOptiXScene;
class UTextureRenderTarget2D;



UCLASS(BlueprintType)
class CARLAOPTIX_API ASceneCaptureComponentNVOptiX :
	public AActor
{
	GENERATED_BODY()
public:

	ASceneCaptureComponentNVOptiX(
		const FObjectInitializer& ObjectInitializer);

	void BeginPlay() override;

	void EndPlay(
		const EEndPlayReason::Type EndPlayReason) override;

	void RenderScene();

private:

	FCarlaOptiXScene* Scene;

	UPROPERTY(EditAnywhere)
	UTextureRenderTarget2D* RenderTargetUE;

};
