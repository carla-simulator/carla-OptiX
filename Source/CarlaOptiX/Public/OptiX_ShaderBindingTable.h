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



struct alignas(OPTIX_SBT_RECORD_ALIGNMENT) FCarlaOptiXSBTRecordHeader
{
	uint8_t Data[OPTIX_SBT_RECORD_HEADER_SIZE];
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

	template <typename... T>
	auto AllocateAndBindRecords(
		FCarlaOptiXProgramGroup& ProgramGroup,
		std::pair<T, ECarlaOptiXSBTRecordKind>&&... Entries)
	{
		constexpr size_t Sizes[] =
		{
			RoundToAlignment(
				sizeof(TCarlaOptiXSBTRecord<std::remove_reference_t<T>>),
				OPTIX_SBT_RECORD_ALIGNMENT)...
		};

		const auto BufferSize = (0 +
			... +
			RoundToAlignment(
				sizeof(TCarlaOptiXSBTRecord<std::remove_reference_t<T>>),
				OPTIX_SBT_RECORD_ALIGNMENT));

		std::array<size_t, sizeof...(T)> Offsets = { };

		size_t Index = 0;
		size_t Offset = 0;
		size_t MissRecordCount = 0;
		size_t HitGroupRecordCount = 0;
		size_t CallablesRecordCount = 0;
		uint8_t* StagingBuffer;
		CheckCUDAResult(cuMemAllocHost((void**)&StagingBuffer, BufferSize));
		([&]() {
			TCarlaOptiXSBTRecord Temp(Entries.first);
			CheckOptiXResult(optixSbtRecordPackHeader(
				ProgramGroup.GetHandle(),
				&Temp));
			(void)memcpy(StagingBuffer + Offset, &Temp, sizeof(Temp));
			Offsets[Index] = Offset;
			++Index;
			Offset += Sizes[Index];
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

		FCarlaOptiXDeviceBuffer Buffer(BufferSize);
		auto Ptr = Buffer.GetDeviceAddress();
		CheckCUDAResult(cuMemFreeHost(StagingBuffer));
		Index = 0;
		([&]() {
			auto Offset = Offsets[Index];
			switch (Entries.second)
			{
			case ECarlaOptiXSBTRecordKind::RayGen:
				BindRayGenRecord(Ptr + Offset);
				break;
			case ECarlaOptiXSBTRecordKind::Exception:
				BindExceptionRecord(Ptr + Offset);
				break;
			case ECarlaOptiXSBTRecordKind::Miss:
				BindMissRecord(Ptr + Offset, Sizes[Index], MissRecordCount);
				break;
			case ECarlaOptiXSBTRecordKind::HitGroup:
				BindHitGroupRecord(Ptr + Offset, Sizes[Index], HitGroupRecordCount);
				break;
			case ECarlaOptiXSBTRecordKind::Callables:
				BindCallablesRecord(Ptr + Offset, Sizes[Index], CallablesRecordCount);
				break;
			default:
				break;
			}
			++Index;
		}(), ...);
		return std::make_tuple(std::move(Buffer), Offsets);
	}

};
