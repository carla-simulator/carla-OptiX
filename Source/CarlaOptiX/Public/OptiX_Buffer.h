#pragma once
#include "OptiX_Common.h"
#include <span>



class CARLAOPTIX_API FCarlaOptiXHostBuffer;
class CARLAOPTIX_API FCarlaOptiXDeviceBuffer;



class CARLAOPTIX_API FCarlaOptiXHostBuffer
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

	constexpr FCarlaOptiXHostBuffer() :
		host_ptr(),
		size()
	{
	}

	explicit FCarlaOptiXHostBuffer(size_t Size);
	explicit FCarlaOptiXHostBuffer(FCarlaOptiXDeviceBuffer& DeviceBuffer);
	FCarlaOptiXHostBuffer(const FCarlaOptiXHostBuffer&) = delete;
	FCarlaOptiXHostBuffer& operator=(const FCarlaOptiXHostBuffer&) = delete;
	FCarlaOptiXHostBuffer(FCarlaOptiXHostBuffer&& rhs);
	FCarlaOptiXHostBuffer& operator=(FCarlaOptiXHostBuffer&& rhs);
	~FCarlaOptiXHostBuffer();
};



class CARLAOPTIX_API FCarlaOptiXDeviceBuffer
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

	constexpr FCarlaOptiXDeviceBuffer() :
		device_ptr(),
		size()
	{
	}

	explicit FCarlaOptiXDeviceBuffer(size_t Size);
	FCarlaOptiXDeviceBuffer(const void* HostData, size_t Size);
	FCarlaOptiXDeviceBuffer(const FCarlaOptiXDeviceBuffer&) = delete;
	FCarlaOptiXDeviceBuffer& operator=(const FCarlaOptiXDeviceBuffer&) = delete;
	FCarlaOptiXDeviceBuffer(FCarlaOptiXDeviceBuffer&& rhs);
	FCarlaOptiXDeviceBuffer& operator=(FCarlaOptiXDeviceBuffer&& rhs);
	~FCarlaOptiXDeviceBuffer();

	template <typename T>
	explicit FCarlaOptiXDeviceBuffer(std::span<T> HostData)
	{
		new (this) FCarlaOptiXDeviceBuffer(
			HostData.data(),
			HostData.size_bytes());
	}

};
