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

	constexpr auto GetData() const
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
		return std::span(GetData(), GetSizeBytes());
	}

	constexpr auto GetSpan() const
	{
		return std::span(GetData(), GetSizeBytes());
	}

	void Destroy();

	constexpr FCarlaOptiXHostBuffer() :
		host_ptr(),
		size()
	{
	}

	explicit FCarlaOptiXHostBuffer(size_t Size);
	explicit FCarlaOptiXHostBuffer(const FCarlaOptiXDeviceBuffer& DeviceBuffer);
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

	constexpr auto GetDeviceAddress() const
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
	FCarlaOptiXDeviceBuffer(const FCarlaOptiXHostBuffer& HostBuffer);
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

	template <typename... T>
	static auto FromValueTuple(T&&... values)
	{
		FCarlaOptiXHostBuffer StagingBuffer((0 + ... + sizeof(T)));
		auto Bump = StagingBuffer.GetData();
		([&]()
		{
			memcpy(Bump, &values, sizeof(T));
			Bump += sizeof(T);
		}, ...);
		return FCarlaOptiXDeviceBuffer(StagingBuffer);
	}

};
