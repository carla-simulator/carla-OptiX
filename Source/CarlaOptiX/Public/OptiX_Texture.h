#pragma once
#include "OptiX_Common.h"



class CARLAOPTIX_API FCarlaOptiXTexture
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

	FCarlaOptiXTexture();
	FCarlaOptiXTexture(const FCarlaOptiXTexture&) = default;
	FCarlaOptiXTexture& operator=(const FCarlaOptiXTexture&) = default;
	FCarlaOptiXTexture(FCarlaOptiXTexture&&);
	FCarlaOptiXTexture& operator=(FCarlaOptiXTexture&&);
	~FCarlaOptiXTexture();

	void Destroy();

	constexpr auto GetShape() const { return Shape; }
	constexpr auto GetWidth() const { return Shape.X; }
	constexpr auto GetHeight() const { return Shape.Y; }
	constexpr auto GetDepth() const { return Shape.Z; }

};
