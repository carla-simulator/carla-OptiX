#pragma once
#include "OptiX_Common.h"
#include "OptiX_ProgramGroup.h"
#include "OptiX_Buffer.h"
#include <array>



class CARLAOPTIX_API FCarlaOptiXInstance;



enum class ECarlaOptiXSBTRecordKind
{
	RayGen,
	Exception,
	Miss,
	HitGroup,
	Callables,
	EnumMax
};



struct alignas(OPTIX_SBT_RECORD_ALIGNMENT) FCarlaOptiXSBTRecordHeader
{
	uint8_t HeaderData[OPTIX_SBT_RECORD_HEADER_SIZE];
};



using FCarlaOptiXEmptyRecord = FCarlaOptiXSBTRecordHeader;



template <typename T>
struct alignas (OPTIX_SBT_RECORD_ALIGNMENT) CARLAOPTIX_API TCarlaOptiXSBTRecord
{
	FCarlaOptiXSBTRecordHeader Header;
	T Data;

	TCarlaOptiXSBTRecord() = default;
	TCarlaOptiXSBTRecord(const TCarlaOptiXSBTRecord&) = default;
	TCarlaOptiXSBTRecord& operator=(const TCarlaOptiXSBTRecord&) = default;
	TCarlaOptiXSBTRecord(TCarlaOptiXSBTRecord&&) = default;
	TCarlaOptiXSBTRecord& operator=(TCarlaOptiXSBTRecord&&) = default;
	~TCarlaOptiXSBTRecord() = default;

	template <typename U>
	requires (std::is_constructible_v<T, U>)
	constexpr TCarlaOptiXSBTRecord(U&& Initializer) :
		Header(),
		Data(std::forward<U>(Initializer))
	{
	}

};

template <typename T>
TCarlaOptiXSBTRecord(T&&) -> TCarlaOptiXSBTRecord<std::remove_reference_t<T>>;



class CARLAOPTIX_API FCarlaOptiXShaderBindingTable
{
	OptixShaderBindingTable SBT;
public:

	FCarlaOptiXShaderBindingTable() = default;
	FCarlaOptiXShaderBindingTable(const FCarlaOptiXShaderBindingTable&) = delete;
	FCarlaOptiXShaderBindingTable& operator=(const FCarlaOptiXShaderBindingTable&) = delete;
	FCarlaOptiXShaderBindingTable(FCarlaOptiXShaderBindingTable&& Other);
	FCarlaOptiXShaderBindingTable& operator=(FCarlaOptiXShaderBindingTable&& Other);
	~FCarlaOptiXShaderBindingTable();

	void Destroy();

	constexpr auto& GetSBT() const { return SBT; }

	void BindRayGenRecord(CUdeviceptr DeviceAddress);
	void BindExceptionRecord(CUdeviceptr DeviceAddress);
	void BindMissRecord(CUdeviceptr DeviceAddress, unsigned Size, unsigned Count = 1);
	void BindHitGroupRecord(CUdeviceptr DeviceAddress, unsigned Size, unsigned Count = 1);
	void BindCallablesRecord(CUdeviceptr DeviceAddress, unsigned Size, unsigned Count = 1);

};
