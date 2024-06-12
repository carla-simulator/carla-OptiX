#include "OptiX_SceneTraceComponent.h"
#include "OptiX_Scene.h"
#include "OptiX_Pipeline.h"
#include "Engine/TextureRenderTarget2D.h"



ASceneTraceComponentNVOptiX::ASceneTraceComponentNVOptiX(
	const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void ASceneTraceComponentNVOptiX::BeginPlay()
{
}

void ASceneTraceComponentNVOptiX::EndPlay(
	const EEndPlayReason::Type EndPlayReason)
{
}

void ASceneTraceComponentNVOptiX::SetPipeline(
	FCarlaOptiXPipeline& NewPipeline)
{
	check(NewPipeline.IsValid());
	Pipeline = std::make_shared<FCarlaOptiXPipeline>(NewPipeline);
}

void ASceneTraceComponentNVOptiX::SetPipeline(
	std::shared_ptr<FCarlaOptiXPipeline> NewPipeline)
{
	check(NewPipeline != nullptr);
	check(NewPipeline->IsValid());
	Pipeline = NewPipeline;
}

void ASceneTraceComponentNVOptiX::TraceRays()
{
	check(Pipeline != nullptr);
	check(Pipeline->IsValid());
	Scene->DispatchKernel(*Pipeline);
}

FCarlaOptiXDeviceArray<FVector3f>& ASceneTraceComponentNVOptiX::GetHitBuffer()
{
	return HitBuffer;
}

FCarlaOptiXHostArray<FVector3f> ASceneTraceComponentNVOptiX::GetHitPositionsHost()
{
	return FCarlaOptiXHostArray<FVector3f>(GetHitBuffer());
}

TArray<FVector> ASceneTraceComponentNVOptiX::GetHitPositions()
{
	TArray<FVector> Result;
	auto HostBuffer = GetHitPositionsHost();
	Result.SetNum(HostBuffer.GetSize());
	for (size_t i = 0; i != Result.Num(); ++i)
		Result[i] = FVector(HostBuffer.GetData()[i]);
	return Result;
}

FUintPoint ASceneTraceComponentNVOptiX::GetShape() const
{
	return Shape;
}
