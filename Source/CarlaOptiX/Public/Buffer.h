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
