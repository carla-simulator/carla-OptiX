#pragma once
#include "CoreMinimal.h"
#include "OptiX_Common.h"
#include "OptiX_Array.h"
#include "OptiX_SceneTraceComponent.generated.h"



class CARLAOPTIX_API FCarlaOptiXScene;
class UTextureRenderTarget2D;



UCLASS(BlueprintType)
class CARLAOPTIX_API ASceneTraceComponentNVOptiX :
	public AActor
{
	GENERATED_BODY()
public:

	ASceneTraceComponentNVOptiX(
		const FObjectInitializer& ObjectInitializer);

	void BeginPlay() override;

	void EndPlay(
		const EEndPlayReason::Type EndPlayReason) override;

	void SetPipeline(FCarlaOptiXPipeline& NewPipeline);

	void SetPipeline(std::shared_ptr<FCarlaOptiXPipeline> NewPipeline);

	void TraceRays();

	FCarlaOptiXDeviceArray<FVector3f>& GetHitBuffer();

	FCarlaOptiXHostArray<FVector3f> GetHitPositionsHost();

	TArray<FVector> GetHitPositions();

	FUintPoint GetShape() const;

private:

	FCarlaOptiXScene* Scene;

	std::shared_ptr<FCarlaOptiXPipeline> Pipeline;

	FCarlaOptiXDeviceArray<FVector3f> HitBuffer;

	FUintPoint Shape;

};
