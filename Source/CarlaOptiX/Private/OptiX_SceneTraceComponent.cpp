#include "OptiX_SceneTraceComponent.h"
#include "OptiX_Instance.h"
#include "OptiX_Scene.h"
#include "OptiX_Pipeline.h"
#include "BuiltinKernelList.h"
#include "Engine/TextureRenderTarget2D.h"
#include <mutex>
#include <atomic>



static std::atomic_uint64_t stc_pipeline_ref_count = 0;
static FCarlaOptiXPipeline STCPipeline;
static FCarlaOptiXProgramGroup STCProgGroup;
static FCarlaOptiXKernelModule STCModule;


struct FLaunchArgs
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

	auto& Instance = FCarlaOptiXInstance::GetGlobalInstanceChecked();

	OptixPayloadType Payload = { };
	Payload.numPayloadValues = GetWordCountOf<FLaunchArgs>;
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
	CompileOptions.numAttributeValues = 0;

	constexpr TCHAR Path[] =
		TEXT("F:/NVOptiXTest/Plugins/carla-OptiX/Source/CarlaOptiX/Private/OptiX-IR/SceneTraceComponent.ptx");

	STCModule = FCarlaOptiXKernelModule::Load(
		Instance,
		Path,
		CompileOptions);

	STCProgGroup = FCarlaOptiXProgramGroup::CreateFromSingleModule<
		EModuleStageKind::RayGen,
		EModuleStageKind::AnyHit,
		EModuleStageKind::ClosestHit,
		EModuleStageKind::Miss,
		EModuleStageKind::Intersection>(
			Instance,
			STCModule);

	STCPipeline = FCarlaOptiXPipeline(
		Instance,
		std::span(&STCProgGroup, 1),
		CompileOptions);
}

static void STCCleanup()
{
#ifdef CARLA_OPTIX_DEBUG
	static std::mutex lock;
	std::scoped_lock guard(lock);
#endif
	STCProgGroup.Destroy();
	STCPipeline.Destroy();
	STCModule.Destroy();
}



ASceneTraceComponentNVOptiX::ASceneTraceComponentNVOptiX(
	const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

ASceneTraceComponentNVOptiX::~ASceneTraceComponentNVOptiX()
{
}

void ASceneTraceComponentNVOptiX::BeginPlay()
{
	Super::BeginPlay();

	if (stc_pipeline_ref_count.fetch_add(1, std::memory_order_acquire) == 0)
		STCInit();

	HitBuffer = FCarlaOptiXDeviceArray<FVector3f>(Width * Height);

	FCarlaOptiXEmptyRecord RayGenRecord = { };
	FCarlaOptiXEmptyRecord AnyHitRecord = { };
	FCarlaOptiXEmptyRecord ClosestHitRecord = { };
	FCarlaOptiXEmptyRecord IntersectionRecord = { };
	FCarlaOptiXEmptyRecord MissRecord = { };
	// FCarlaOptiXEmptyRecord ExceptionRecord = { };
	// FCarlaOptiXEmptyRecord CallablesRecord = { };

	STCProgGroup.PackHeaders(
		RayGenRecord,
		AnyHitRecord,
		ClosestHitRecord,
		IntersectionRecord,
		MissRecord);

	SBTRecords = FCarlaOptiXDeviceBuffer::FromValueTuple(
		RayGenRecord,
		AnyHitRecord,
		ClosestHitRecord,
		IntersectionRecord,
		MissRecord);

	SBT = FCarlaOptiXShaderBindingTable();
	SBT.BindRayGenRecord(
		SBTRecords.GetDeviceAddress());
	SBT.BindHitGroupRecord(
		SBTRecords.GetDeviceAddress(),
		sizeof(AnyHitRecord) + sizeof(ClosestHitRecord) + sizeof(IntersectionRecord));
	SBT.BindMissRecord(
		SBTRecords.GetDeviceAddress(),
		sizeof(MissRecord));
	// SBT.BindExceptionRecord(SBTRecords.GetDeviceAddress());
	// SBT.BindCallablesRecord(SBTRecords.GetDeviceAddress());
}

void ASceneTraceComponentNVOptiX::Tick(float dt)
{
	Trace();
}

void ASceneTraceComponentNVOptiX::EndPlay(
	const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (stc_pipeline_ref_count.load(std::memory_order_acquire) != 0)
		if (stc_pipeline_ref_count.fetch_sub(1, std::memory_order_acquire) - 1 == 0)
			STCCleanup();
}

void ASceneTraceComponentNVOptiX::Trace()
{
	FLaunchArgs Args;
	FCarlaOptiXDeviceBuffer Buffer(sizeof(Args));
	cuMemcpyHtoD(Buffer.GetDeviceAddress(), &Args, sizeof(Args));
	STCPipeline.Launch(SBT, Buffer, FUintVector3(Width, Height, 1));
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
	return static_cast<int32>(Width);
}

int32 ASceneTraceComponentNVOptiX::GetHeight() const
{
	return static_cast<int32>(Height);
}

FUintPoint ASceneTraceComponentNVOptiX::GetShape() const
{
	return FUintPoint(Width, Height);
}

void ASceneTraceComponentNVOptiX::SetShape(int32 NewWidth, int32 NewHeight)
{
	check(NewWidth > 0);
	check(NewHeight > 0);
	Width = (uint32)NewWidth;
	Height = (uint32)NewHeight;
	HitBuffer = FCarlaOptiXDeviceArray<FVector3f>(Width * Height);
}
