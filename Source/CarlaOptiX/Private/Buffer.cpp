#include "Buffer.h"



FOptixHostBuffer::FOptixHostBuffer(size_t size) :
	staging_buffer(),
	size(size)
{
	check(size != 0);
	CheckCUDAResult(cuMemAllocHost(reinterpret_cast<void**>(&staging_buffer), size));
}

FOptixHostBuffer::FOptixHostBuffer(FOptixHostBuffer&& rhs) :
	staging_buffer(rhs.staging_buffer),
	size(rhs.size)
{
	rhs.staging_buffer = nullptr;
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
	if (staging_buffer != nullptr)
		Destroy();
}

void FOptixHostBuffer::Destroy()
{
	CheckCUDAResult(cuMemFreeHost(staging_buffer));
	memset(this, 0, sizeof(FOptixHostBuffer));
}



FOptixDeviceBuffer::FOptixDeviceBuffer(size_t size) :
	data(),
	size(size)
{
	CheckCUDAResult(cuMemAlloc(&data, size));
}

FOptixDeviceBuffer::FOptixDeviceBuffer(FOptixDeviceBuffer&& rhs) :
	data(rhs.data),
	size(rhs.size)
{
	rhs.data = CUdeviceptr();
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
	if (data != CUdeviceptr())
		Destroy();
}

void FOptixDeviceBuffer::Destroy()
{
	check(data != CUdeviceptr());
	CheckCUDAResult(cuMemFree(data));
	memset(this, 0, sizeof(FOptixDeviceBuffer));
}
