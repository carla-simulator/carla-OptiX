#include "OptiX_Buffer.h"



FOptixHostBuffer::FOptixHostBuffer(size_t size) :
	host_ptr(),
	size(size)
{
	check(size != 0);
	CARLA_OPTIX_LOG_VERBOSE(TEXT("Allocating host buffer (size=%llu)"), (unsigned long long)size);
	CheckCUDAResult(cuMemAllocHost(reinterpret_cast<void**>(&host_ptr), size));
}

FOptixHostBuffer::FOptixHostBuffer(FOptixHostBuffer&& rhs) :
	host_ptr(rhs.host_ptr),
	size(rhs.size)
{
	rhs.host_ptr = nullptr;
	rhs.size = 0;
}

FOptixHostBuffer& FOptixHostBuffer::operator=(FOptixHostBuffer&& rhs)
{
	this->~FOptixHostBuffer();
	new (this) FOptixHostBuffer(std::move(rhs));
	return *this;
}

FOptixHostBuffer::~FOptixHostBuffer()
{
	if (host_ptr != nullptr)
		Destroy();
}

void FOptixHostBuffer::Destroy()
{
	CARLA_OPTIX_LOG_VERBOSE(
		TEXT("Destroying host buffer (address=0x%p, size=%llu)"),
		GetData(),
		GetSizeBytes());
	CheckCUDAResult(cuMemFreeHost(host_ptr));
	memset(this, 0, sizeof(FOptixHostBuffer));
}



FOptixDeviceBuffer::FOptixDeviceBuffer(
	size_t size) :
	device_ptr(),
	size(size)
{
	CARLA_OPTIX_LOG_VERBOSE(
		TEXT("Allocating device buffer (%llu bytes)"),
		(unsigned long long)size);
	CheckCUDAResult(cuMemAlloc(&device_ptr, size));
}

FOptixDeviceBuffer::FOptixDeviceBuffer(const void* HostData, size_t Size)
{
	new (this) FOptixDeviceBuffer(Size);
	
	CARLA_OPTIX_LOG_VERBOSE(
		TEXT("Copying %llu bytes from host to device memory (0x%llx)."),
		(unsigned long long)GetSizeBytes(),
		(unsigned long long)GetDeviceAddress());
	CheckCUDAResult(
		cuMemcpyHtoD(
			GetDeviceAddress(),
			HostData,
			Size));
}

FOptixDeviceBuffer::FOptixDeviceBuffer(FOptixDeviceBuffer&& rhs) :
	device_ptr(rhs.device_ptr),
	size(rhs.size)
{
	rhs.device_ptr = CUdeviceptr();
	rhs.size = 0;
}

FOptixDeviceBuffer& FOptixDeviceBuffer::operator=(FOptixDeviceBuffer&& rhs)
{
	this->~FOptixDeviceBuffer();
	new (this) FOptixDeviceBuffer(std::move(rhs));
	return *this;
}

FOptixDeviceBuffer::~FOptixDeviceBuffer()
{
	if (device_ptr != CUdeviceptr())
		Destroy();
}

void FOptixDeviceBuffer::Destroy()
{
	check(device_ptr != CUdeviceptr());
	CARLA_OPTIX_LOG_VERBOSE(
		TEXT("Destroying device buffer (device_address=0x%llx, size=%llu)"),
		(unsigned long long)GetDeviceAddress(),
		(unsigned long long)GetSizeBytes());
	CheckCUDAResult(cuMemFree(device_ptr));
	memset(this, 0, sizeof(FOptixDeviceBuffer));
}
