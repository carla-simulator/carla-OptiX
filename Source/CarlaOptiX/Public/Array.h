#pragma once
#include "Buffer.h"



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
