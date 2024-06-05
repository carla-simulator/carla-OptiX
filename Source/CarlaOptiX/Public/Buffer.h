#pragma once
#include "OptiXHeaders.h"
#include <span>



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

	void Destroy();

	constexpr FOptixHostBuffer() :
		staging_buffer(),
		size()
	{
	}

	FOptixHostBuffer(size_t size);
	FOptixHostBuffer(const FOptixHostBuffer&) = delete;
	FOptixHostBuffer& operator=(const FOptixHostBuffer&) = delete;
	FOptixHostBuffer(FOptixHostBuffer&& rhs);
	FOptixHostBuffer& operator=(FOptixHostBuffer&& rhs);
	~FOptixHostBuffer();

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

	void Destroy();

	constexpr FOptixDeviceBuffer() :
		data(),
		size()
	{
	}

	FOptixDeviceBuffer(size_t size);
	FOptixDeviceBuffer(const FOptixDeviceBuffer&) = delete;
	FOptixDeviceBuffer& operator=(const FOptixDeviceBuffer&) = delete;
	FOptixDeviceBuffer(FOptixDeviceBuffer&& rhs);
	FOptixDeviceBuffer& operator=(FOptixDeviceBuffer&& rhs);
	~FOptixDeviceBuffer();
	
};
