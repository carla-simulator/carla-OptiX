#include "OptiX_Scene.h"
#include "OptiX_Instance.h"
#include "OptiX_Pipeline.h"
#include "Kismet/GameplayStatics.h"
#include "Algo/Accumulate.h"

FCarlaOptiXScene::FCarlaOptiXScene()
{
	memset(this, 0, sizeof(FCarlaOptiXScene));
}

FCarlaOptiXScene::FCarlaOptiXScene(FCarlaOptiXInstance* Instance)
{
	memset(this, 0, sizeof(FCarlaOptiXScene));
	this->OptixInstance = Instance;
	check(Instance);
}

void FCarlaOptiXScene::EnumerateBuildInputs(
	std::vector<OptixBuildInput>& OutBuildInputs,
	std::vector<CUdeviceptr>& OutPointers,
	std::vector<unsigned>& OutFlags)
{
	OutBuildInputs.resize(StaticMeshes.size());
	OutPointers.resize(StaticMeshes.size());
	OutFlags.resize(StaticMeshes.size());
	for (size_t i = 0; i != StaticMeshes.size(); ++i)
	{
		auto& StaticMesh = StaticMeshes[i];
		auto& Input = OutBuildInputs[i];
		auto& Pointer = OutPointers[i];
		auto& Flags = OutFlags[i];
		auto& IndexBuffer = StaticMesh.GetIndexBuffer();
		auto& VertexBuffer = StaticMesh.GetPositionBuffer();
		auto& TriangleArray = Input.triangleArray;

		Flags = OPTIX_GEOMETRY_FLAG_NONE;
		Pointer = VertexBuffer.GetDeviceAddress();
		Input.type = OPTIX_BUILD_INPUT_TYPE_TRIANGLES;
		TriangleArray.vertexBuffers = &Pointer;
		TriangleArray.numVertices = VertexBuffer.GetSize();
		TriangleArray.vertexFormat = StaticMesh.GetVertexFormat();
		TriangleArray.vertexStrideInBytes = StaticMesh.GetVertexStride();
		TriangleArray.indexBuffer = IndexBuffer.GetDeviceAddress();
		check((IndexBuffer.GetSize() % 3) == 0);
		TriangleArray.numIndexTriplets = IndexBuffer.GetSize() / 3;
		TriangleArray.indexFormat = StaticMesh.GetIndexFormat(); // ???
		TriangleArray.indexStrideInBytes = StaticMesh.GetIndexStride();
		TriangleArray.preTransform = CUdeviceptr();
		TriangleArray.numSbtRecords = 1;
		TriangleArray.flags = &Flags;
		TriangleArray.sbtIndexOffsetBuffer = CUdeviceptr();
		TriangleArray.sbtIndexOffsetSizeInBytes = 0;
		TriangleArray.sbtIndexOffsetStrideInBytes = 0;
		TriangleArray.primitiveIndexOffset = 0;
		TriangleArray.transformFormat = OPTIX_TRANSFORM_FORMAT_NONE;
		TriangleArray.opacityMicromap = OptixBuildInputOpacityMicromap();
		TriangleArray.displacementMicromap = OptixBuildInputDisplacementMicromap();
	}
}

void FCarlaOptiXScene::AddSceneStaticMeshes(UWorld* Source)
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(
		Source,
		AActor::StaticClass(),
		Actors);
	TArray<UActorComponent*> Components;
	for (auto Actor : Actors)
	{
		auto ActorComponents = Actor->GetComponents();
		Components.Reserve(Components.Num() + ActorComponents.Num());
		for (auto Component : ActorComponents)
			Components.Add(Component);
	}
	Actors.Empty();
	TArray<UStaticMesh*> Meshes;
	for (auto Component : Components)
		if (auto Mesh = Cast<UStaticMeshComponent>(Component); Mesh != nullptr)
			Meshes.Add(Mesh->GetStaticMesh());
	Components.Empty();
	CARLA_OPTIX_LOG_VERBOSE(
		TEXT("FCarlaOptiXScene(%p): Found %llu StaticMeshes."),
		this,
		(unsigned long long)Meshes.Num());
	for (auto Mesh : Meshes)
		StaticMeshes.push_back(FCarlaOptiXStaticMesh(Mesh));
}

void FCarlaOptiXScene::BuildGAS()
{
	CARLA_OPTIX_LOG_VERBOSE(
		TEXT("FCarlaOptiXScene (%p): GAS build started."),
		this);

	std::vector<OptixAccelBufferSizes> BufferSizes;
	std::vector<OptixBuildInput> BuildInputs;
	std::vector<CUdeviceptr> Pointers;
	std::vector<unsigned> Flags;
	EnumerateBuildInputs(BuildInputs, Pointers, Flags);

	BufferSizes.resize(BuildInputs.size());
	OptixAccelBuildOptions Options = { };
	Options.buildFlags = 0;
	Options.operation = OPTIX_BUILD_OPERATION_BUILD;
	Options.motionOptions = { };
	CheckOptiXResult(optixAccelComputeMemoryUsage(
		OptixInstance->GetOptixDeviceContext(),
		&Options,
		BuildInputs.data(),
		BuildInputs.size(),
		BufferSizes.data()));

	auto OutSize = Algo::Accumulate<size_t>(BufferSizes, 0, [](size_t l, auto& r)
		{
			return l + r.outputSizeInBytes;
		});

	auto TempSize = Algo::Accumulate<size_t>(BufferSizes, 0, [](size_t l, auto& r)
		{
			return l + r.tempSizeInBytes;
		});

	CARLA_OPTIX_LOG_VERBOSE(
		TEXT("FCarlaOptiXScene(%p): Computed GAS memory size (out=%llu, temp=%llu)."),
		this,
		(unsigned long long)OutSize,
		(unsigned long long)TempSize);

	FCarlaOptiXDeviceBuffer TempGASBuffer(OutSize);
	OptixTraversableHandle TempGAS;

	{
		FCarlaOptiXDeviceBuffer ScratchBuffer(TempSize);
		CheckOptiXResult(optixAccelBuild(
			OptixInstance->GetOptixDeviceContext(),
			0,
			&Options,
			BuildInputs.data(),
			BuildInputs.size(),
			ScratchBuffer.GetDeviceAddress(),
			ScratchBuffer.GetSizeBytes(),
			TempGASBuffer.GetDeviceAddress(),
			TempGASBuffer.GetSizeBytes(),
			&TempGAS,
			nullptr,
			0));
	}

	CARLA_OPTIX_LOG_VERBOSE(
		TEXT("FCarlaOptiXScene(%p): GAS build finished (handle=%llx), compacting..."),
		this,
		(unsigned long long)TempGAS);

	GASBuffer = FCarlaOptiXDeviceBuffer(OutSize);
	CheckOptiXResult(optixAccelCompact(
		OptixInstance->GetOptixDeviceContext(),
		0,
		TempGAS,
		GASBuffer.GetDeviceAddress(),
		GASBuffer.GetSizeBytes(),
		&GeometryAccelerationStructure));

	CARLA_OPTIX_LOG_VERBOSE(
		TEXT("FCarlaOptiXScene(%p): GAS compaction finished (old_handle=%llx, final_handle=%llx)."),
		this,
		(unsigned long long)TempGAS,
		(unsigned long long)GeometryAccelerationStructure);
}

FCarlaOptiXScene::FCarlaOptiXScene(FCarlaOptiXScene&& Other)
{
	(void)memcpy(this, &Other, sizeof(FCarlaOptiXScene));
	(void)memset(&Other, 0, sizeof(FCarlaOptiXScene));
}

FCarlaOptiXScene& FCarlaOptiXScene::operator=(FCarlaOptiXScene&& Other)
{
	this->~FCarlaOptiXScene();
	new (this) FCarlaOptiXScene(std::move(Other));
	return *this;
}

FCarlaOptiXScene::~FCarlaOptiXScene()
{
#if 0
	for (auto& Mesh : StaticMeshes)
	{
		CARLA_OPTIX_LOG_VERBOSE(TEXT("Mesh %p"), &Mesh);
		CARLA_OPTIX_LOG_VERBOSE(TEXT("%s"), *Mesh.DebugDumpInfoString());
	}
#endif
}

void FCarlaOptiXScene::UpdateFromWorld(
	UWorld* Source)
{
	CARLA_OPTIX_LOG_VERBOSE(
		TEXT("FCarlaOptiXScene(%p): Updating from UWorld %p."),
		this,
		Source);
	AddSceneStaticMeshes(Source);
	BuildGAS();
}

void FCarlaOptiXScene::DispatchKernel(
	FCarlaOptiXPipeline& Pipeline)
{
}



ACarlaOptiXScene::ACarlaOptiXScene(
	const FObjectInitializer& Initializer) :
	Super(Initializer)
{
}

void ACarlaOptiXScene::Initialize()
{
	auto Instance = FCarlaOptiXInstance::GetGlobalInstance();
	check(Instance);
	Implementation = FCarlaOptiXScene(Instance);
}

void ACarlaOptiXScene::UpdateFromWorld(UWorld* Source)
{
	check(Source);
	check(Implementation.IsValid());
	Implementation.UpdateFromWorld(Source);
}

void ACarlaOptiXScene::UpdateFromCurrentWorld()
{
	UpdateFromWorld(GetWorld());
}
