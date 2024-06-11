#pragma once
#include "OptiX_Common.h"



class CARLAOPTIX_API FCarlaOptiXMaterial
{
public:

	FCarlaOptiXMaterial() = default;
	FCarlaOptiXMaterial(const FCarlaOptiXMaterial&) = delete;
	FCarlaOptiXMaterial& operator=(const FCarlaOptiXMaterial&) = delete;
	FCarlaOptiXMaterial(FCarlaOptiXMaterial&&) = default;
	FCarlaOptiXMaterial& operator=(FCarlaOptiXMaterial&&) = default;
	~FCarlaOptiXMaterial() = default;

};
