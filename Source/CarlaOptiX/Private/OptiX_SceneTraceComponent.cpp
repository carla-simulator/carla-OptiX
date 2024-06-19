#include "OptiX_SceneTraceComponent.h"
#include "OptiX_Instance.h"
#include "OptiX_Scene.h"
#include "OptiX_Pipeline.h"
#include "BuiltinKernelList.h"
#include "Engine/TextureRenderTarget2D.h"



struct FLaunchParameters
{
	OptixTraversableHandle gas;
	FVector3f* result_buffer;
	FVector3f origin;
	FVector3f direction;
	FUintPoint viewport_size;
	float ray_time;
	float epsilon;
	float max_distance;
};

ASceneTraceComponentNVOptiX::ASceneTraceComponentNVOptiX(
	const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void ASceneTraceComponentNVOptiX::BeginPlay()
{
	OptixPayloadType Payload;
	Payload.numPayloadValues = GetWordCountOf<FLaunchParameters>;
	unsigned PayloadSemantics[] =
	{
		OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ |
		OPTIX_PAYLOAD_SEMANTICS_CH_READ |
		OPTIX_PAYLOAD_SEMANTICS_MS_READ |
		OPTIX_PAYLOAD_SEMANTICS_AH_READ |
		OPTIX_PAYLOAD_SEMANTICS_IS_READ
	};
	Payload.payloadSemantics = PayloadSemantics;

	auto& Instance = FCarlaOptiXInstance::GetGlobalInstanceChecked();

	OptixPipelineCompileOptions Options;
	Options.usesMotionBlur = false;
	Options.traversableGraphFlags =
		OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_GAS;
	Options.numPayloadValues = 2;
	Options.numAttributeValues = 2;
	Options.exceptionFlags = OPTIX_EXCEPTION_FLAG_NONE;
#ifdef CARLA_OPTIX_DEBUG
	Options.exceptionFlags =
		OPTIX_EXCEPTION_FLAG_STACK_OVERFLOW |
		OPTIX_EXCEPTION_FLAG_TRACE_DEPTH |
		OPTIX_EXCEPTION_FLAG_USER;
#endif
	Options.pipelineLaunchParamsVariableName = "args";
	Options.usesPrimitiveTypeFlags = OPTIX_PRIMITIVE_TYPE_FLAGS_TRIANGLE;
	Options.allowOpacityMicromaps = false;

	auto [NewPipeline, NewModule] = FCarlaOptiXPipeline::CreateFromTraits<
		EModuleStageKind::RayGen,
		EModuleStageKind::AnyHit,
		EModuleStageKind::ClosestHit,
		EModuleStageKind::Miss,
		EModuleStageKind::Intersection>(
			Instance,
			TEXT("F:/NVOptiXTest/Plugins/carla-OptiX/Source/CarlaOptiX/Private/OptiX-IR/SceneTraceComponent.ptx"),
			Options,
			std::span(&Payload, 1));

	SetPipeline(std::move(NewPipeline));
	SetModule(std::move(NewModule));
}

void ASceneTraceComponentNVOptiX::EndPlay(
	const EEndPlayReason::Type EndPlayReason)
{
	Module.reset();
	Pipeline.reset();
}

void ASceneTraceComponentNVOptiX::SetPipeline(
	FCarlaOptiXPipeline&& NewPipeline)
{
	check(NewPipeline.IsValid());
	Pipeline = std::make_shared<FCarlaOptiXPipeline>(std::move(NewPipeline));
}

void ASceneTraceComponentNVOptiX::SetPipeline(
	std::shared_ptr<FCarlaOptiXPipeline> NewPipeline)
{
	check(NewPipeline != nullptr);
	check(NewPipeline->IsValid());
	Pipeline = NewPipeline;
}

void ASceneTraceComponentNVOptiX::SetModule(
	FCarlaOptiXKernelModule&& NewModule)
{
	check(NewModule.IsValid());
	Module = std::make_shared<FCarlaOptiXKernelModule>(std::move(NewModule));
}

void ASceneTraceComponentNVOptiX::SetModule(
	std::shared_ptr<FCarlaOptiXKernelModule> NewModule)
{
	check(NewModule != nullptr);
	check(NewModule->IsValid());
	Module = NewModule;
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

const FCarlaOptiXDeviceArray<FVector3f>& ASceneTraceComponentNVOptiX::GetHitBuffer() const
{
	return HitBuffer;
}

FCarlaOptiXHostArray<FVector3f> ASceneTraceComponentNVOptiX::GetHitPositionsHost() const
{
	return FCarlaOptiXHostArray<FVector3f>(GetHitBuffer());
}

TArray<FVector> ASceneTraceComponentNVOptiX::GetHitPositions() const
{
	TArray<FVector> Result;
	auto HostBuffer = GetHitPositionsHost();
	Result.SetNum(HostBuffer.GetSize());
	for (size_t i = 0; i != Result.Num(); ++i)
		Result[i] = FVector(HostBuffer.GetData()[i]);
	return Result;
}

int32 ASceneTraceComponentNVOptiX::GetWidth() const
{
	return static_cast<int32>(GetShape().X);
}

int32 ASceneTraceComponentNVOptiX::GetHeight() const
{
	return static_cast<int32>(GetShape().Y);
}

FUintPoint ASceneTraceComponentNVOptiX::GetShape() const
{
	return Shape;
}
