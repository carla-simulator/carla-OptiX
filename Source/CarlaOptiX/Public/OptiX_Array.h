#pragma once
#include "OptiX_Buffer.h"



template <typename T>
class CARLAOPTIX_API FCarlaOptiXHostArray;

template <typename T>
class CARLAOPTIX_API FCarlaOptiXDeviceArray;



template <typename T>
class CARLAOPTIX_API FCarlaOptiXHostArray :
	public FCarlaOptiXHostBuffer
{
public:

	using FCarlaOptiXHostBuffer::GetSizeBytes;

	constexpr auto GetSize() const
	{
		return FCarlaOptiXHostBuffer::GetSizeBytes() / sizeof(T);
	}

	constexpr auto GetData()
	{
		return reinterpret_cast<T*>(FCarlaOptiXHostBuffer::GetData());
	}

	constexpr auto GetSpan()
	{
		return std::span(GetData(), GetSize());
	}

	FCarlaOptiXHostArray() = default;

	explicit FCarlaOptiXHostArray(size_t count) :
		FCarlaOptiXHostBuffer(count * sizeof(T))
	{
	}

	template <typename T>
	explicit FCarlaOptiXHostArray(FCarlaOptiXDeviceArray<T>& Source) :
		FCarlaOptiXHostBuffer(Source)
	{
	}

	FCarlaOptiXHostArray(const FCarlaOptiXHostArray&) = delete;
	FCarlaOptiXHostArray& operator=(const FCarlaOptiXHostArray&) = delete;
	FCarlaOptiXHostArray(FCarlaOptiXHostArray&&) = default;
	FCarlaOptiXHostArray& operator=(FCarlaOptiXHostArray&&) = default;
	~FCarlaOptiXHostArray() = default;
};



template <typename T>
class CARLAOPTIX_API FCarlaOptiXDeviceArray :
	public FCarlaOptiXDeviceBuffer
{
public:

	using FCarlaOptiXDeviceBuffer::GetDeviceAddress;
	using FCarlaOptiXDeviceBuffer::GetSizeBytes;

	constexpr auto GetSize() const
	{
		return FCarlaOptiXDeviceBuffer::GetSizeBytes() / sizeof(T);
	}

	explicit FCarlaOptiXDeviceArray(size_t count) :
		FCarlaOptiXDeviceBuffer(count * sizeof(T))
	{
	}

	explicit FCarlaOptiXDeviceArray(std::span<T> SourceSpan) :
		FCarlaOptiXDeviceBuffer(SourceSpan)
	{
	}

	FCarlaOptiXDeviceArray() = default;
	FCarlaOptiXDeviceArray(const FCarlaOptiXDeviceArray&) = delete;
	FCarlaOptiXDeviceArray& operator=(const FCarlaOptiXDeviceArray&) = delete;
	FCarlaOptiXDeviceArray(FCarlaOptiXDeviceArray&&) = default;
	FCarlaOptiXDeviceArray& operator=(FCarlaOptiXDeviceArray&&) = default;
	~FCarlaOptiXDeviceArray() = default;

};