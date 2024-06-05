#pragma once
#include "OptiXHeaders.h"



class CARLAOPTIX_API FOptixStaticMesh
{
	FOptixDeviceArray<uint32_t> Indices;
	FOptixDeviceArray<FVector3f> Positions;
	FOptixDeviceArray<FVector3f> Normals;
	FOptixDeviceArray<FVector3f> Tangents;
	FOptixDeviceArray<FVector3f> TextureUVs;
	std::shared_ptr<FOptixMaterial> Material;

public:

	FOptixStaticMesh() = default;
	FOptixStaticMesh(const FOptixStaticMesh&) = delete;
	FOptixStaticMesh& operator=(const FOptixStaticMesh&) = delete;
	FOptixStaticMesh(FOptixStaticMesh&&) = default;
	FOptixStaticMesh& operator=(FOptixStaticMesh&&) = default;
	~FOptixStaticMesh() = default;

};



class CARLAOPTIX_API FOptixMesh
{
	TArray<FOptixStaticMesh> Primitives; // Using TArray causes problems with move-only types.
public:

	FOptixMesh() = default;
	FOptixMesh(const FOptixMesh&) = delete;
	FOptixMesh& operator=(const FOptixMesh&) = delete;
	FOptixMesh(FOptixMesh&&) = default;
	FOptixMesh& operator=(FOptixMesh&&) = default;
	~FOptixMesh() = default;

};
