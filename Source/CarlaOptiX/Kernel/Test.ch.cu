


extern "C" __global__ void __closesthit__ch()
{
  // When built-in triangle intersection is used, a number of fundamental 
  // attributes are provided by the OptiX API, including barycentric 
  // coordinates.
  const float2 barycentrics = optixGetTriangleBarycentrics();
  // Convert to color and assign to our payload outputs.
  const float3 c = make_float3(barycentrics, 1.0f); 
  optixSetPayload_0(float_as_int(c.x));
  optixSetPayload_1(float_as_int(c.y));
  optixSetPayload_2(float_as_int(c.z));
}
