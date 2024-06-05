#include "OptiX_StaticMesh.h"
#include "Engine/StaticMesh.h"
#undef min
#undef max



FCarlaOptiXStaticMesh::FCarlaOptiXStaticMesh(
	UStaticMesh* Source,
	int32 LODIndex)
{
	Initialize(Source, LODIndex);
}

void FCarlaOptiXStaticMesh::Initialize(
	UStaticMesh* Source,
	int32 LODIndex)
{
	check(IsValid(Source));
	auto RenderData = Source->GetRenderData();
	check(RenderData);
	check(LODIndex < RenderData->LODResources.Num());

	auto& LODResource = RenderData->LODResources[LODIndex];
	auto& VertexBuffers = LODResource.VertexBuffers;
	auto& PositionBuffer = VertexBuffers.PositionVertexBuffer;
	auto& IndexBuffer = LODResource.IndexBuffer;

	auto IndexCount = IndexBuffer.GetNumIndices();
	auto VertexCount = PositionBuffer.GetNumVertices();
	auto IndexBufferSize = IndexCount * sizeof(uint32);
	auto VertexBufferSize = VertexCount * sizeof(FVector3f);

	FOptixHostBuffer StagingBuffer(std::max(
		IndexBufferSize,
		VertexBufferSize));

	std::span<uint32> IndicesStagingSpan(
		reinterpret_cast<uint32*>(StagingBuffer.GetData()),
		IndexCount);

	std::span<FVector3f> VerticesStagingSpan(
		reinterpret_cast<FVector3f*>(StagingBuffer.GetData()),
		VertexCount);

	for (uint32 i = 0; i != IndexCount; ++i)
		IndicesStagingSpan[i] = IndexBuffer.GetIndex(i);

	Indices = FOptixDeviceArray<uint32>(IndicesStagingSpan);

	for (uint32 i = 0; i != VertexCount; ++i)
		VerticesStagingSpan[i] = PositionBuffer.VertexPosition(i);

	Positions = FOptixDeviceArray<FVector3f>(VerticesStagingSpan);
}

void FCarlaOptiXStaticMesh::Destroy()
{
	Indices.Destroy();
	Positions.Destroy();
	Normals.Destroy();
	Tangents.Destroy();
	TextureUVs.Destroy();
	Material.reset();
}



ACarlaOptixStaticMesh::ACarlaOptixStaticMesh(
	const FObjectInitializer& Initializer) :
	Super(Initializer),
	FCarlaOptiXStaticMesh()
{
}

void ACarlaOptixStaticMesh::Initialize(
	UStaticMesh* Source,
	int32 LODIndex)
{
	FCarlaOptiXStaticMesh::Destroy();
	FCarlaOptiXStaticMesh::Initialize(Source, LODIndex);
}
