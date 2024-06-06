#include "OptiX_Scene.h"
#include "OptiX_Instance.h"
#include "Kismet/GameplayStatics.h"

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
	std::vector<CUdeviceptr>& OutPointers)
{
	OutBuildInputs.resize(StaticMeshes.size());
	OutPointers.resize(StaticMeshes.size());
	for (size_t i = 0; i != StaticMeshes.size(); ++i)
	{
		auto& StaticMesh = StaticMeshes[i];
		auto& Input = OutBuildInputs[i];
		auto& Pointer = OutPointers[i];
		auto& IndexBuffer = StaticMesh.GetIndexBuffer();
		auto& VertexBuffer = StaticMesh.GetPositionBuffer();
		Pointer = VertexBuffer.GetDeviceAddress();
		Input.type = OPTIX_BUILD_INPUT_TYPE_TRIANGLES;
		auto& TriangleArray = Input.triangleArray;
		TriangleArray.vertexBuffers = &Pointer;
		TriangleArray.numVertices = 1;
		TriangleArray.vertexFormat = StaticMesh.GetVertexFormat();
		TriangleArray.vertexStrideInBytes = sizeof(FVector3f);
		TriangleArray.indexBuffer = IndexBuffer.GetDeviceAddress();
		check((IndexBuffer.GetSize() % 3) == 0);
		TriangleArray.numIndexTriplets = IndexBuffer.GetSize() / 3;
		TriangleArray.indexFormat = OPTIX_INDICES_FORMAT_UNSIGNED_INT3; // ???
		TriangleArray.indexStrideInBytes = 0;
		TriangleArray.preTransform = CUdeviceptr();
		TriangleArray.flags = nullptr;
		TriangleArray.numSbtRecords = 1;
		TriangleArray.sbtIndexOffsetBuffer = CUdeviceptr();
		TriangleArray.sbtIndexOffsetSizeInBytes = 0;
		TriangleArray.sbtIndexOffsetStrideInBytes = 0;
		TriangleArray.primitiveIndexOffset = 0;
		TriangleArray.transformFormat = OPTIX_TRANSFORM_FORMAT_NONE;
		TriangleArray.opacityMicromap = OptixBuildInputOpacityMicromap();
		TriangleArray.displacementMicromap = OptixBuildInputDisplacementMicromap();
	}
}

void FCarlaOptiXScene::BuildGAS()
{
	std::vector<OptixAccelBufferSizes> BufferSizes;
	std::vector<OptixBuildInput> BuildInputs;
	std::vector<CUdeviceptr> Pointers;
	EnumerateBuildInputs(BuildInputs, Pointers);

	OptixAccelBuildOptions Options = { };
	Options.buildFlags = 0;
	Options.operation = OPTIX_BUILD_OPERATION_BUILD;
	Options.motionOptions = { };

	BufferSizes.resize(BuildInputs.size());
	CheckOptiXResult(optixAccelComputeMemoryUsage(
		OptixInstance->GetOptixDeviceContext(),
		&Options,
		BuildInputs.data(),
		BuildInputs.size(),
		BufferSizes.data()));

	size_t out_size = 0;
	size_t temp_size = 0;
	for (auto& BufferSize : BufferSizes)
	{
		out_size += BufferSize.outputSizeInBytes;
		temp_size += BufferSize.tempSizeInBytes;
	}

	GASBuffer = FOptixDeviceBuffer(out_size);
	FOptixDeviceBuffer ScratchBuffer(temp_size);
	FOptixDeviceBuffer TempBuffer(temp_size);
	OptixTraversableHandle TempGAS;
	CheckOptiXResult(optixAccelBuild(
		OptixInstance->GetOptixDeviceContext(),
		0,
		&Options,
		BuildInputs.data(),
		BuildInputs.size(),
		ScratchBuffer.GetDeviceAddress(),
		ScratchBuffer.GetSizeBytes(),
		TempBuffer.GetDeviceAddress(),
		TempBuffer.GetSizeBytes(),
		&TempGAS,
		nullptr,
		0));
	
	CheckOptiXResult(optixAccelCompact(
		OptixInstance->GetOptixDeviceContext(),
		0,
		TempGAS,
		GASBuffer.GetDeviceAddress(),
		GASBuffer.GetSizeBytes(),
		&GeometryAccelerationStructure));
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
	for (auto Mesh : Meshes)
		StaticMeshes.push_back(FCarlaOptiXStaticMesh(Mesh));
}

FCarlaOptiXScene::FCarlaOptiXScene(FCarlaOptiXScene&& Other)
{
	memcpy(this, &Other, sizeof(FCarlaOptiXScene));
	memset(&Other, 0, sizeof(FCarlaOptiXScene));
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

void FCarlaOptiXScene::UpdateFromWorld(UWorld* Source)
{
	AddSceneStaticMeshes(Source);
	BuildGAS();
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
	check(Implementation.IsValid());
	Implementation.UpdateFromWorld(Source);
}

void ACarlaOptiXScene::UpdateFromCurrentWorld()
{
	auto World = GetWorld();
	check(World);
	UpdateFromWorld(World);
}
