#pragma once
#include "OptiX_Common.h"
#include "OptiX_Array.h"
#include "OptiX_Material.h"
#include <memory>
#include "OptiX_StaticMesh.generated.h"



class CARLAOPTIX_API FCarlaOptiXStaticMesh
{
	FOptixDeviceArray<uint32> Indices;
	FOptixDeviceArray<FVector3f> Positions;
	// FOptixDeviceArray<FVector3f> Normals;
	// FOptixDeviceArray<FVector3f> Tangents;
	// FOptixDeviceArray<FVector3f> TextureUVs;
	// std::shared_ptr<FOptixMaterial> Material;

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

	void Initialize(
		UStaticMesh* Source,
		int32 LODIndex = 0);

	void Destroy();

	void DebugDumpData(FString OutPath);

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



class CARLAOPTIX_API FOptixMesh
{
	TArray<FCarlaOptiXStaticMesh> Primitives; // Using TArray causes problems with move-only types.
public:

	FOptixMesh() = default;
	FOptixMesh(const FOptixMesh&) = delete;
	FOptixMesh& operator=(const FOptixMesh&) = delete;
	FOptixMesh(FOptixMesh&&) = default;
	FOptixMesh& operator=(FOptixMesh&&) = default;
	~FOptixMesh() = default;

};
