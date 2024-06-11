#pragma once
#include "OptiX_Common.h"
#include "OptiX_ProgramGroup.h"
#include <array>



class CARLAOPTIX_API FCarlaOptiXInstance;



enum class ECarlaOptiXSBTRecordKind
{
	RayGen,
	Exception,
	Miss,
	HitGroup,
	Callables
};



template <typename T>
struct alignas (OPTIX_SBT_RECORD_ALIGNMENT) CARLAOPTIX_API TCarlaOptiXSBTRecord
{
	char Header[OPTIX_SBT_RECORD_HEADER_SIZE];
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

	template <typename... T>
	static CUdeviceptr AllocateAndBindRecords(
		FCarlaOptiXProgramGroup& ProgramGroup,
		std::pair<T, ECarlaOptiXSBTRecordKind>&&... Entries)
	{
		const auto BufferSize =
			(... + sizeof(TCarlaOptiXSBTRecord<std::remove_reference_t<T>>));

		size_t Offsets[sizeof...(T)];

		size_t Index = 0;
		size_t Offset = 0;
		size_t MissRecordCount = 0;
		size_t HitGroupRecordCount = 0;
		size_t CallablesRecordCount = 0;
		uint8_t* StagingBuffer;
		CheckCUDAResult(cuMemAllocHost((void**)&StagingBuffer, BufferSize));
		([] {
			TCarlaOptiXSBTRecord Temp(Entries.first);
			CheckOptiXResult(optixSbtRecordPackHeader(
				ProgramGroup.GetHandle(),
				&Temp));
			(void)memcpy(StagingBuffer + Offset, &Temp, sizeof(Temp));
			Offsets[Index] = Offset;
			++Index;
			Offset += sizeof(Temp);
			switch (Entries.second)
			{
			case ECarlaOptiXSBTRecordKind::Miss:
				MissRecordCount++;
				break;
			case ECarlaOptiXSBTRecordKind::HitGroup:
				HitGroupRecordCount++;
				break;
			case ECarlaOptiXSBTRecordKind::Callables:
				CallablesRecordCount++;
				break;
			default:
				break;
			}
		}(), ...);

		CUdeviceptr Ptr;
		CheckCUDAResult(cuMemAlloc(&Ptr, BufferSize));
		CheckCUDAResult(cuMemFreeHost(StagingBuffer));
		Index = 0;
		([] {
			switch (Entries.second)
			{
			case ECarlaOptiXSBTRecordKind::RayGen:
				BindRayGenRecord(Ptr + Offset);
				break;
			case ECarlaOptiXSBTRecordKind::Exception:
				BindExceptionRecord(Ptr + Offset);
				break;
			case ECarlaOptiXSBTRecordKind::Miss:
				BindMissRecord(Ptr + Offset, MissRecordCount);
				break;
			case ECarlaOptiXSBTRecordKind::HitGroup:
				BindHitGroupRecord(Ptr + Offset, HitGroupRecordCount);
				break;
			case ECarlaOptiXSBTRecordKind::Callables:
				BindCallablesRecord(Ptr + Offset, CallablesRecordCount);
				break;
			default:
				break;
			}
			++Index;
		}(), ...);
		return Ptr;
	}

};
