#pragma once
#include "OptiXHeaders.h"



class CARLAOPTIX_API FOptixMaterial
{
public:

	FOptixMaterial() = default;
	FOptixMaterial(const FOptixMaterial&) = delete;
	FOptixMaterial& operator=(const FOptixMaterial&) = delete;
	FOptixMaterial(FOptixMaterial&&) = default;
	FOptixMaterial& operator=(FOptixMaterial&&) = default;
	~FOptixMaterial() = default;

};