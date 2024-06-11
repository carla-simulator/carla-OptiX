#include "OptiX_ShaderBindingTable.h"

FCarlaOptiXShaderBindingTable::FCarlaOptiXShaderBindingTable(FCarlaOptiXShaderBindingTable&& Other) :
	SBT(Other.SBT)
{
	Other.SBT = OptixShaderBindingTable();
}

FCarlaOptiXShaderBindingTable& FCarlaOptiXShaderBindingTable::operator=(FCarlaOptiXShaderBindingTable&& Other)
{
	this->~FCarlaOptiXShaderBindingTable();
	new (this) FCarlaOptiXShaderBindingTable(std::move(Other));
	return *this;
}

FCarlaOptiXShaderBindingTable::~FCarlaOptiXShaderBindingTable()
{
}

void FCarlaOptiXShaderBindingTable::Destroy()
{
}

void FCarlaOptiXShaderBindingTable::BindRayGenRecord(CUdeviceptr DeviceAddress)
{
	SBT.raygenRecord = DeviceAddress;
}

void FCarlaOptiXShaderBindingTable::BindExceptionRecord(CUdeviceptr DeviceAddress)
{
	SBT.exceptionRecord = DeviceAddress;
}

void FCarlaOptiXShaderBindingTable::BindMissRecord(
	CUdeviceptr DeviceAddress,
	unsigned Size,
	unsigned Count)
{
	SBT.missRecordBase = DeviceAddress;
	SBT.missRecordCount = Count;
	SBT.missRecordStrideInBytes = Size;
}

void FCarlaOptiXShaderBindingTable::BindHitGroupRecord(
	CUdeviceptr DeviceAddress,
	unsigned Size,
	unsigned Count)
{
	SBT.hitgroupRecordBase = DeviceAddress;
	SBT.hitgroupRecordCount = Count;
	SBT.hitgroupRecordStrideInBytes = Size;
}

void FCarlaOptiXShaderBindingTable::BindCallablesRecord(
	CUdeviceptr DeviceAddress,
	unsigned Size,
	unsigned Count)
{
	SBT.callablesRecordBase = DeviceAddress;
	SBT.callablesRecordCount = Count;
	SBT.callablesRecordStrideInBytes = Size;
}
