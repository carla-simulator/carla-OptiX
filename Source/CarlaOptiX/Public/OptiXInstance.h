#pragma once
#include <optix.h>
#include <optix.h>
#include <optix_denoiser_tiling.h>
#include <optix_function_table_definition.h>
#include <optix_function_table.h>
// #include <optix_micromap.h> // This requires defining vector types.
#include <optix_stack_size.h>
#include <optix_stubs.h>
#include <optix_types.h>

#include <span>
#include <memory>
#include <source_location>



void CheckCUDAResult(
	CUresult ec,
	std::source_location loc = std::source_location::current());

void CheckOptiXResult(
	OptixResult ec,
	std::source_location loc = std::source_location::current());



struct FCarlaOptiXInstanceOptions
{
	size_t CUDADeviceIndex = 0;
};



class CARLAOPTIX_API FCarlaOptiXInstance
{
	CUcontext CUDAContext;
	CUdevice CUDADevice;
	OptixDeviceContext OptixContext;

	static void OptixCallback(
		unsigned int level,
		const char* tag,
		const char* message,
		void* self);

public:

	static void InitGlobalContext();
	static void DestroyGlobalContext();

	FCarlaOptiXInstance();
	FCarlaOptiXInstance(
		const FCarlaOptiXInstanceOptions& optix_options);
	FCarlaOptiXInstance(const FCarlaOptiXInstance&) = delete;
	FCarlaOptiXInstance& operator=(const FCarlaOptiXInstance&) = delete;
	~FCarlaOptiXInstance();

	constexpr auto GetCUDAContext() { return CUDAContext; }
	constexpr auto GetOptixDeviceContext() { return OptixContext; }

};



class CARLAOPTIX_API FOptixTexture
{
	CUtexObject Texture;
	union
	{
		CUarray Array;
		CUmipmappedArray MipMappedArray;
	};
	uint32_t HostEncoding;
	uint32_t DeviceEncoding;
	CUDA_ARRAY3D_DESCRIPTOR ArrayDescriptor;
	CUDA_RESOURCE_DESC ResourceDescriptor;
	CUDA_TEXTURE_DESC TextureDescriptor;
	FUintVector3 Shape;
	size_t ElementSize;
	size_t BufferSize;
public:

	FOptixTexture();

	constexpr auto GetShape() const { return Shape; }
	constexpr auto GetWidth() const { return Shape.X; }
	constexpr auto GetHeight() const { return Shape.Y; }
	constexpr auto GetDepth() const { return Shape.Z; }

};



struct CARLAOPTIX_API FOptixPointLightInfo
{
	FTransform Transform;
	FVector3d Color;
	float Intensity;
	float Range;
};



struct CARLAOPTIX_API FOptixSpotLightInfo
{
	FTransform Transform;
	FVector3d Color;
	float Intensity;
	float Range;
	float InnerConeAngle;
	float OuterConeAngle;
};



class CARLAOPTIX_API FOptixHostBuffer
{
	uint8_t* staging_buffer;
	size_t size;
public:

	constexpr auto GetHostPointer()
	{
		return staging_buffer;
	}

	constexpr auto GetSize() const
	{
		return size;
	}

	constexpr auto GetSizeBytes() const
	{
		return size;
	}

	constexpr auto GetHostSpan()
	{
		return std::span(staging_buffer, GetSizeBytes());
	}

	void Destroy()
	{
		CheckCUDAResult(cuMemFreeHost(staging_buffer));
		memset(this, 0, sizeof(FOptixHostBuffer));
	}

	constexpr FOptixHostBuffer() :
		staging_buffer(),
		size()
	{
	}

	FOptixHostBuffer(size_t size) :
		staging_buffer(),
		size(size)
	{
		check(size != 0);
		CheckCUDAResult(cuMemAllocHost(reinterpret_cast<void**>(&staging_buffer), size));
	}

	FOptixHostBuffer(const FOptixHostBuffer&) = delete;
	FOptixHostBuffer& operator=(const FOptixHostBuffer&) = delete;

	constexpr FOptixHostBuffer(FOptixHostBuffer&& rhs) :
		staging_buffer(rhs.staging_buffer),
		size(rhs.size)
	{
		rhs.staging_buffer = nullptr;
		rhs.size = 0;
	}

	inline FOptixHostBuffer& operator=(FOptixHostBuffer&& rhs)
	{
		this->~FOptixHostBuffer();
		new (this) FOptixHostBuffer(std::move(rhs));
		return *this;
	}

	inline ~FOptixHostBuffer()
	{
		if (staging_buffer != nullptr)
			Destroy();
	}
};



class CARLAOPTIX_API FOptixDeviceBuffer
{
	CUdeviceptr data;
	size_t size;

public:

	constexpr auto GetDevicePointer()
	{
		return data;
	}

	constexpr auto GetSize() const
	{
		return size;
	}

	constexpr auto GetSizeBytes() const
	{
		return size;
	}

	void Destroy()
	{
		check(data != CUdeviceptr());
		CheckCUDAResult(cuMemFree(data));
		memset(this, 0, sizeof(FOptixDeviceBuffer));
	}

	constexpr FOptixDeviceBuffer() :
		data(),
		size()
	{
	}

	FOptixDeviceBuffer(size_t size) :
		data(),
		size(size)
	{
		CheckCUDAResult(cuMemAlloc(&data, size));
	}

	FOptixDeviceBuffer(const FOptixDeviceBuffer&) = delete;
	FOptixDeviceBuffer& operator=(const FOptixDeviceBuffer&) = delete;

	constexpr FOptixDeviceBuffer(FOptixDeviceBuffer&& rhs) :
		data(rhs.data),
		size(rhs.size)
	{
		rhs.data = CUdeviceptr();
		rhs.size = 0;
	}

	inline FOptixDeviceBuffer& operator=(FOptixDeviceBuffer&& rhs)
	{
		this->~FOptixDeviceBuffer();
		new (this) FOptixDeviceBuffer(std::move(rhs));
		return *this;
	}

	inline ~FOptixDeviceBuffer()
	{
		if (data != CUdeviceptr())
			Destroy();
	}
};



template <typename T>
class CARLAOPTIX_API FOptixDeviceArray :
	public FOptixDeviceBuffer
{
public:

	using FOptixDeviceBuffer::GetDevicePointer;
	using FOptixDeviceBuffer::GetSizeBytes;

	constexpr auto GetSize() const
	{
		return FOptixDeviceBuffer::GetSizeBytes() * sizeof(T);
	}

	FOptixDeviceArray(size_t count) :
		FOptixDeviceBuffer(count * sizeof(T))
	{
	}

	FOptixDeviceArray() = default;
	FOptixDeviceArray(const FOptixDeviceArray&) = delete;
	FOptixDeviceArray& operator=(const FOptixDeviceArray&) = delete;
	FOptixDeviceArray(FOptixDeviceArray&&) = default;
	FOptixDeviceArray& operator=(FOptixDeviceArray&&) = default;
	~FOptixDeviceArray() = default;

};



template <typename T>
class CARLAOPTIX_API FOptixHostArray :
	public FOptixHostBuffer
{
public:

	using FOptixHostBuffer::GetHostPointer;
	using FOptixHostBuffer::GetSizeBytes;

	constexpr auto GetSize() const
	{
		return FOptixHostBuffer::GetSizeBytes() * sizeof(T);
	}

	FOptixHostArray(size_t count) :
		FOptixHostBuffer(count * sizeof(T))
	{
	}

	FOptixHostArray() = default;
	FOptixHostArray(const FOptixHostArray&) = delete;
	FOptixHostArray& operator=(const FOptixHostArray&) = delete;
	FOptixHostArray(FOptixHostArray&&) = default;
	FOptixHostArray& operator=(FOptixHostArray&&) = default;
	~FOptixHostArray() = default;

};



class CARLAOPTIX_API FOptixMaterial
{
public:

	FOptixMaterial() = default;
	FOptixMaterial(const FOptixMaterial&) = delete;
	FOptixMaterial& operator=(const FOptixMaterial&) = delete;
	FOptixMaterial(FOptixMaterial&&) = default;
	FOptixMaterial& operator=(FOptixMaterial&&) = default;
	~FOptixMaterial() = default;

};



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



class CARLAOPTIX_API FCarlaOptiXScene
{
	FCarlaOptiXInstance* OptixInstance;
	FScene* Scene;

public:

	FCarlaOptiXScene(
		FCarlaOptiXInstance& OptixInstance);
	FCarlaOptiXScene(const FCarlaOptiXScene&) = delete;
	FCarlaOptiXScene& operator=(const FCarlaOptiXScene&) = delete;
	~FCarlaOptiXScene();

	constexpr auto GetOptixInstance() { return OptixInstance; }

};

