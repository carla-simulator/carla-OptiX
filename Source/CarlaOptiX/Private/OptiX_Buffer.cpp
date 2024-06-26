#include "OptiX_Buffer.h"



FCarlaOptiXHostBuffer::FCarlaOptiXHostBuffer(size_t size) :
	host_ptr(),
	size(size)
{
	check(size != 0);
	CARLA_OPTIX_LOG_VERBOSE(TEXT("Allocating host buffer (size=%llu)"), (unsigned long long)size);
	CheckCUDAResult(cuMemAllocHost(reinterpret_cast<void**>(&host_ptr), size));
}

FCarlaOptiXHostBuffer::FCarlaOptiXHostBuffer(
	const FCarlaOptiXDeviceBuffer& DeviceBuffer)
{
	new (this) FCarlaOptiXHostBuffer(DeviceBuffer.GetSize());
	CARLA_OPTIX_LOG_VERBOSE(
		TEXT("Copying %llu bytes from device to host memory (0x%llx->0x%p)."),
		(unsigned long long)DeviceBuffer.GetSizeBytes(),
		(unsigned long long)DeviceBuffer.GetDeviceAddress(),
		GetData());
	CheckCUDAResult(cuMemcpyDtoH(GetData(), DeviceBuffer.GetDeviceAddress(), DeviceBuffer.GetSizeBytes()));
}

FCarlaOptiXHostBuffer::FCarlaOptiXHostBuffer(FCarlaOptiXHostBuffer&& rhs) :
	host_ptr(rhs.host_ptr),
	size(rhs.size)
{
	rhs.host_ptr = nullptr;
	rhs.size = 0;
}

FCarlaOptiXHostBuffer& FCarlaOptiXHostBuffer::operator=(FCarlaOptiXHostBuffer&& rhs)
{
	this->~FCarlaOptiXHostBuffer();
	new (this) FCarlaOptiXHostBuffer(std::move(rhs));
	return *this;
}

FCarlaOptiXHostBuffer::~FCarlaOptiXHostBuffer()
{
	if (host_ptr != nullptr)
		Destroy();
}

void FCarlaOptiXHostBuffer::Destroy()
{
	CARLA_OPTIX_LOG_VERBOSE(
		TEXT("Destroying host buffer (address=0x%p, size=%llu)"),
		GetData(),
		GetSizeBytes());
	CheckCUDAResult(cuMemFreeHost(host_ptr));
	memset(this, 0, sizeof(FCarlaOptiXHostBuffer));
}



FCarlaOptiXDeviceBuffer::FCarlaOptiXDeviceBuffer(
	size_t size) :
	device_ptr(),
	size(size)
{
	CARLA_OPTIX_LOG_VERBOSE(
		TEXT("Allocating device buffer (%llu bytes)"),
		(unsigned long long)size);
	CheckCUDAResult(cuMemAlloc(&device_ptr, size));
}

FCarlaOptiXDeviceBuffer::FCarlaOptiXDeviceBuffer(const void* HostData, size_t Size)
{
	new (this) FCarlaOptiXDeviceBuffer(Size);

	CARLA_OPTIX_LOG_VERBOSE(
		TEXT("Copying %llu bytes from host to device memory (0x%p->0x%llx)."),
		(unsigned long long)GetSizeBytes(),
		HostData,
		(unsigned long long)GetDeviceAddress());
	CheckCUDAResult(
		cuMemcpyHtoD(
			GetDeviceAddress(),
			HostData,
			Size));
}

FCarlaOptiXDeviceBuffer::FCarlaOptiXDeviceBuffer(const FCarlaOptiXHostBuffer& HostBuffer)
{
	new (this) FCarlaOptiXDeviceBuffer(HostBuffer.GetSpan());
}

FCarlaOptiXDeviceBuffer::FCarlaOptiXDeviceBuffer(FCarlaOptiXDeviceBuffer&& rhs) :
	device_ptr(rhs.device_ptr),
	size(rhs.size)
{
	rhs.device_ptr = CUdeviceptr();
	rhs.size = 0;
}

FCarlaOptiXDeviceBuffer& FCarlaOptiXDeviceBuffer::operator=(FCarlaOptiXDeviceBuffer&& rhs)
{
	this->~FCarlaOptiXDeviceBuffer();
	new (this) FCarlaOptiXDeviceBuffer(std::move(rhs));
	return *this;
}

FCarlaOptiXDeviceBuffer::~FCarlaOptiXDeviceBuffer()
{
	if (device_ptr != CUdeviceptr())
		Destroy();
}

void FCarlaOptiXDeviceBuffer::Destroy()
{
	check(device_ptr != CUdeviceptr());
	CARLA_OPTIX_LOG_VERBOSE(
		TEXT("Destroying device buffer (device_address=0x%llx, size=%llu)"),
		(unsigned long long)GetDeviceAddress(),
		(unsigned long long)GetSizeBytes());
	CheckCUDAResult(cuMemFree(device_ptr));
	memset(this, 0, sizeof(FCarlaOptiXDeviceBuffer));
}
