#pragma once
#include "CoreMinimal.h"
#include "OptiX_Common.h"
#include "OptiX_Array.h"
#include "OptiX_SceneTraceComponent.generated.h"



class CARLAOPTIX_API FCarlaOptiXScene;
class CARLAOPTIX_API FCarlaOptiXPipeline;
class CARLAOPTIX_API FCarlaOptiXKernelModule;
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

	void TraceRays();

	FCarlaOptiXDeviceArray<FVector3f>& GetHitBuffer();

	const FCarlaOptiXDeviceArray<FVector3f>& GetHitBuffer() const;

	FCarlaOptiXHostArray<FVector3f> GetHitPositionsHost() const;

	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetHitPositions() const;

	UFUNCTION(BlueprintCallable)
	int32 GetWidth() const;

	UFUNCTION(BlueprintCallable)
	int32 GetHeight() const;

	FUintPoint GetShape() const;

private:

	FCarlaOptiXScene* Scene;

	FCarlaOptiXDeviceArray<FVector3f> HitBuffer;

	FUintPoint Shape;

};
