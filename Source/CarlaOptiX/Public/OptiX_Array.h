#pragma once
#include "OptiX_Buffer.h"



template <typename T>
class CARLAOPTIX_API FOptixHostArray :
	public FOptixHostBuffer
{
public:

	using FOptixHostBuffer::GetSizeBytes;

	constexpr auto GetSize() const
	{
		return FOptixHostBuffer::GetSizeBytes() / sizeof(T);
	}

	constexpr auto GetData()
	{
		return reinterpret_cast<T*>(FOptixHostBuffer::GetData());
	}

	constexpr auto GetSpan()
	{
		return std::span(GetData(), GetSize());
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

	using FOptixDeviceBuffer::GetDeviceAddress;
	using FOptixDeviceBuffer::GetSizeBytes;

	constexpr auto GetSize() const
	{
		return FOptixDeviceBuffer::GetSizeBytes() / sizeof(T);
	}

	explicit FOptixDeviceArray(size_t count) :
		FOptixDeviceBuffer(count * sizeof(T))
	{
	}

	explicit FOptixDeviceArray(std::span<T> SourceSpan) :
		FOptixDeviceBuffer(SourceSpan)
	{
	}

	FOptixDeviceArray() = default;
	FOptixDeviceArray(const FOptixDeviceArray&) = delete;
	FOptixDeviceArray& operator=(const FOptixDeviceArray&) = delete;
	FOptixDeviceArray(FOptixDeviceArray&&) = default;
	FOptixDeviceArray& operator=(FOptixDeviceArray&&) = default;
	~FOptixDeviceArray() = default;

};