#include <optix.h>



struct KernelArgs
{
	OptixTraversableHandle gas;
	float3* result_buffer;
	float3 origin;
	float3 direction;
	uint2 viewport_size;
	float ray_time;
	float epsilon;
	float max_distance;
};

struct RayContext
{
	float3 hit_position;
	float3 hit_tangent;
	float3 hit_normal;
};



extern "C" __constant__ KernelArgs args;



#define MAKE_POINTER(low, high)			(void*)((uintptr_t)(low) | ((uintptr_t)(high) << 32))
#define POINTER_LOW32(ptr) (uint32_t)((uintptr_t)(ptr) & UINT32_MAX)
#define POINTER_HIGH32(ptr) (uint32_t)(((uintptr_t)(ptr) >> 32) & UINT32_MAX)



extern "C" __global__ void __raygen__main()
{
	const uint3 launch_id = optixGetLaunchIndex();
	const uint32_t sbt_offset = 0;
	const uint32_t sbt_stride = 0;
	const uint32_t sbt_miss_index = 0;

	RayContext ray_ctx;
	auto low32 = POINTER_LOW32(&ray_ctx);
	auto high32 = POINTER_HIGH32(&ray_ctx);

	optixTrace(
		args.gas,
		args.origin,
		args.direction,
		args.epsilon,
		args.max_distance,
		OptixVisibilityMask(0xFF),
		0,
		sbt_offset,
		sbt_stride,
		sbt_miss_index,
		low32,
		high32);
}

extern "C" __global__ void __anyhit__main()
{
	const uint3 launch_id = optixGetLaunchIndex();
}

extern "C" __global__ void __closesthit__main()
{
	const uint3 launch_id = optixGetLaunchIndex();
	auto& ray_ctx = *(RayContext*)MAKE_POINTER(
		optixGetPayload_0(),
		optixGetPayload_1());
}

extern "C" __global__ void __miss__main()
{
	const uint3 launch_id = optixGetLaunchIndex();
	auto& ray_ctx = *(RayContext*)MAKE_POINTER(
		optixGetPayload_0(),
		optixGetPayload_1());
}

extern "C" __global__ void __intersection__main()
{
	const uint3 launch_id = optixGetLaunchIndex();
	auto& ray_ctx = *(RayContext*)MAKE_POINTER(
		optixGetPayload_0(),
		optixGetPayload_1());
}