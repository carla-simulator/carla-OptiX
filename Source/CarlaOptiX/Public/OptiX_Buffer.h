#pragma once
#include "OptiX_Common.h"
#include <span>



class CARLAOPTIX_API FOptixHostBuffer;
class CARLAOPTIX_API FOptixDeviceBuffer;



class CARLAOPTIX_API FOptixHostBuffer
{
	uint8_t* host_ptr;
	size_t size;
public:

	constexpr auto GetData()
	{
		return host_ptr;
	}

	constexpr auto GetSize() const
	{
		return size;
	}

	constexpr auto GetSizeBytes() const
	{
		return size;
	}

	constexpr auto GetSpan()
	{
		return std::span(host_ptr, GetSizeBytes());
	}

	void Destroy();

	constexpr FOptixHostBuffer() :
		host_ptr(),
		size()
	{
	}

	explicit FOptixHostBuffer(size_t Size);
	explicit FOptixHostBuffer(FOptixDeviceBuffer& DeviceBuffer);
	FOptixHostBuffer(const FOptixHostBuffer&) = delete;
	FOptixHostBuffer& operator=(const FOptixHostBuffer&) = delete;
	FOptixHostBuffer(FOptixHostBuffer&& rhs);
	FOptixHostBuffer& operator=(FOptixHostBuffer&& rhs);
	~FOptixHostBuffer();
};



class CARLAOPTIX_API FOptixDeviceBuffer
{
	CUdeviceptr device_ptr;
	size_t size;

public:

	constexpr auto GetDeviceAddress()
	{
		return device_ptr;
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
		device_ptr(),
		size()
	{
	}

	explicit FOptixDeviceBuffer(size_t Size);
	FOptixDeviceBuffer(const void* HostData, size_t Size);
	FOptixDeviceBuffer(const FOptixDeviceBuffer&) = delete;
	FOptixDeviceBuffer& operator=(const FOptixDeviceBuffer&) = delete;
	FOptixDeviceBuffer(FOptixDeviceBuffer&& rhs);
	FOptixDeviceBuffer& operator=(FOptixDeviceBuffer&& rhs);
	~FOptixDeviceBuffer();

	template <typename T>
	explicit FOptixDeviceBuffer(std::span<T> HostData)
	{
		new (this) FOptixDeviceBuffer(
			HostData.data(),
			HostData.size_bytes());
	}

};
