#pragma once
#include "CoreMinimal.h"
#include "OptiX_Common.h"
#include "OptiX_Array.h"
#include "OptiX_ShaderBindingTable.h"
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

	~ASceneTraceComponentNVOptiX();

	UFUNCTION(BlueprintCallable)
	void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void Tick(float dt) override;

	UFUNCTION(BlueprintCallable)
	void EndPlay(
		const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void Trace();

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

	UFUNCTION(BlueprintCallable)
	void SetShape(int32 NewWidth, int32 NewHeight);

private:

	FCarlaOptiXScene* Scene;

	FCarlaOptiXDeviceArray<FVector3f> HitBuffer;

	FCarlaOptiXShaderBindingTable SBT;

	UPROPERTY(EditAnywhere)
	uint32 Width = 800;

	UPROPERTY(EditAnywhere)
	uint32 Height = 600;

};
