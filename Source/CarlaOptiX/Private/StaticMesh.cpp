#include "StaticMesh.h"
#include "Engine/StaticMesh.h"



FOptixStaticMesh::FOptixStaticMesh(
	UStaticMesh* Source,
	int32 LODIndex)
{
	check(IsValid(Source));
	auto RenderData = Source->GetRenderData();
	check(RenderData);
	check(LODIndex < RenderData->LODResources.Num());
	auto& LODResource = RenderData->LODResources[LODIndex];
	auto VertexCount = LODResource.GetNumVertices();
	auto& VertexBuffers = LODResource.VertexBuffers;
	auto& PositionBuffer = VertexBuffers.PositionVertexBuffer;
	for (int32 i = 0; i != VertexCount; ++i)
	{
		auto Position = PositionBuffer.VertexPosition(i);
	}
	auto NaniteVertexCount = Source->GetNumNaniteVertices();
	auto RenderData = Source->GetRenderData();
	RenderData->LODResources[LODIndex].GetNumVertices();
}
