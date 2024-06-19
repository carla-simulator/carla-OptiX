#include "OptiX_SceneTraceComponent.h"
#include "OptiX_Instance.h"
#include "OptiX_Scene.h"
#include "OptiX_Pipeline.h"
#include "BuiltinKernelList.h"
#include "Engine/TextureRenderTarget2D.h"
#include <atomic>



static std::atomic_uint64_t stc_pipeline_ref_count = 0;
static FCarlaOptiXPipeline STCPipeline;
static FCarlaOptiXKernelModule STCModule;


struct FLaunchParameters
{
	OptixTraversableHandle gas;
	DevicePtr<FVector3f> result_buffer;
	FVector3f origin;
	FVector3f direction;
	FUintPoint viewport_size;
	float ray_time;
	float epsilon;
	float max_distance;
};



static void STCInit()
{
	const unsigned PayloadSemantics[] =
	{
		OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ_WRITE |
		OPTIX_PAYLOAD_SEMANTICS_CH_READ |
		OPTIX_PAYLOAD_SEMANTICS_MS_READ |
		OPTIX_PAYLOAD_SEMANTICS_AH_READ |
		OPTIX_PAYLOAD_SEMANTICS_IS_READ
	};

	OptixPayloadType Payload = { };
	Payload.numPayloadValues = GetWordCountOf<FLaunchParameters>;
	Payload.payloadSemantics = PayloadSemantics;

	OptixPipelineCompileOptions CompileOptions = { };
	CompileOptions.traversableGraphFlags = OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_GAS;
#ifdef CARLA_OPTIX_DEBUG
	CompileOptions.exceptionFlags =
		OPTIX_EXCEPTION_FLAG_STACK_OVERFLOW |
		OPTIX_EXCEPTION_FLAG_TRACE_DEPTH |
		OPTIX_EXCEPTION_FLAG_USER;
#else
	CompileOptions.exceptionFlags = OPTIX_EXCEPTION_FLAG_NONE;
#endif
	CompileOptions.pipelineLaunchParamsVariableName = "args";
	CompileOptions.usesPrimitiveTypeFlags = OPTIX_PRIMITIVE_TYPE_FLAGS_TRIANGLE;
	CompileOptions.allowOpacityMicromaps = false;
	CompileOptions.usesMotionBlur = false;
	CompileOptions.numPayloadValues = GetWordCountOf<void*>;
	CompileOptions.numAttributeValues = 0; // ?

	constexpr TCHAR Path[] = TEXT("F:/NVOptiXTest/Plugins/carla-OptiX/Source/CarlaOptiX/Private/OptiX-IR/SceneTraceComponent.ptx");

	auto [Pipeline, Module] = FCarlaOptiXPipeline::CreateFromTraits<
		EModuleStageKind::RayGen,
		EModuleStageKind::AnyHit,
		EModuleStageKind::ClosestHit,
		EModuleStageKind::Miss,
		EModuleStageKind::Intersection>(
			FCarlaOptiXInstance::GetGlobalInstanceChecked(),
			Path,
			CompileOptions);

	STCPipeline = std::move(Pipeline);
	STCModule = std::move(Module);
}

static void STCCleanup()
{
	STCModule.Destroy();
	STCPipeline.Destroy();
}



ASceneTraceComponentNVOptiX::ASceneTraceComponentNVOptiX(
	const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

ASceneTraceComponentNVOptiX::~ASceneTraceComponentNVOptiX()
{
	if (stc_pipeline_ref_count.fetch_sub(1, std::memory_order_acquire) - 1 == 0)
		STCCleanup();
}

void ASceneTraceComponentNVOptiX::BeginPlay()
{
	if (stc_pipeline_ref_count.fetch_add(1, std::memory_order_acquire) == 0)
		STCInit();
}

void ASceneTraceComponentNVOptiX::EndPlay(
	const EEndPlayReason::Type EndPlayReason)
{
}

void ASceneTraceComponentNVOptiX::TraceRays()
{
	// STCPipeline.Launch();
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
