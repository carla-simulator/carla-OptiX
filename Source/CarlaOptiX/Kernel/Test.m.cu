extern "C" __global__ void __miss__ms()
{
  MissData* miss_data = reinterpret_cast<MissData*>(optixGetSbtDataPointer());
  setPayload(miss_data->bg_color);
}
