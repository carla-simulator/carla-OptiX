#pragma once
#include <optix.h>
#include <optix_denoiser_tiling.h>
#include <optix_function_table_definition.h>
#include <optix_function_table.h>
// #include <optix_micromap.h> // This requires defining vector types.
#include <optix_stack_size.h>
#include <optix_stubs.h>
#include <optix_types.h>

#include <source_location>



void CheckCUDAResult(
	CUresult ec,
	std::source_location loc = std::source_location::current());

void CheckOptiXResult(
	OptixResult ec,
	std::source_location loc = std::source_location::current());