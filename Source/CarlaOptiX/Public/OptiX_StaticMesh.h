#pragma once
#include "OptiX_Common.h"
#include "OptiX_Array.h"
#include "OptiX_Material.h"
#include <memory>
#include "OptiX_StaticMesh.generated.h"



class CARLAOPTIX_API FCarlaOptiXStaticMesh
{
	FCarlaOptiXDeviceArray<uint32> Indices;
	FCarlaOptiXDeviceArray<FVector3f> Positions;
	// FCarlaOptiXDeviceArray<FVector3f> Normals;
	// FCarlaOptiXDeviceArray<FVector3f> Tangents;
	// FCarlaOptiXDeviceArray<FVector3f> TextureUVs;
	// std::shared_ptr<FCarlaOptiXMaterial> Material;

public:

	FCarlaOptiXStaticMesh() = default;
	explicit FCarlaOptiXStaticMesh(
		UStaticMesh* Source,
		int32 LODIndex = 0);
	FCarlaOptiXStaticMesh(const FCarlaOptiXStaticMesh&) = delete;
	FCarlaOptiXStaticMesh& operator=(const FCarlaOptiXStaticMesh&) = delete;
	FCarlaOptiXStaticMesh(FCarlaOptiXStaticMesh&&) = default;
	FCarlaOptiXStaticMesh& operator=(FCarlaOptiXStaticMesh&&) = default;
	~FCarlaOptiXStaticMesh() = default;

	constexpr auto& GetIndexBuffer() { return Indices; }
	constexpr auto& GetPositionBuffer() { return Positions; }

	static constexpr auto GetIndexFormat() { return OPTIX_INDICES_FORMAT_UNSIGNED_INT3; }
	static constexpr auto GetVertexFormat() { return OPTIX_VERTEX_FORMAT_FLOAT3; }
	static constexpr auto GetVertexStride() { return sizeof(FVector3f); }
	static constexpr auto GetIndexStride() { return sizeof(uint32) * 3; }

	void Initialize(
		UStaticMesh* Source,
		int32 LODIndex = 0);

	void Destroy();

	FString DebugDumpInfoString();

};



UCLASS()
class CARLAOPTIX_API ACarlaOptixStaticMesh :
	public AActor,
	public FCarlaOptiXStaticMesh
{
	GENERATED_BODY()
public:

	ACarlaOptixStaticMesh(const FObjectInitializer&);

	UFUNCTION(BlueprintCallable)
	void Initialize(
		UStaticMesh* Source,
		int32 LODIndex = 0);

};



class CARLAOPTIX_API FCarlaOptiXMesh
{
	TArray<FCarlaOptiXStaticMesh> Primitives; // Using TArray causes problems with move-only types.
public:

	FCarlaOptiXMesh() = default;
	FCarlaOptiXMesh(const FCarlaOptiXMesh&) = delete;
	FCarlaOptiXMesh& operator=(const FCarlaOptiXMesh&) = delete;
	FCarlaOptiXMesh(FCarlaOptiXMesh&&) = default;
	FCarlaOptiXMesh& operator=(FCarlaOptiXMesh&&) = default;
	~FCarlaOptiXMesh() = default;

};
