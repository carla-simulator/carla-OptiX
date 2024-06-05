#pragma once
#include "OptiXHeaders.h"
#include "CarlaOptiX.h"



class CARLAOPTIX_API FCarlaOptiXScene
{
	FCarlaOptiXInstance* OptixInstance;
	FScene* Scene;

public:

	FCarlaOptiXScene(
		FCarlaOptiXInstance& OptixInstance);
	FCarlaOptiXScene(const FCarlaOptiXScene&) = delete;
	FCarlaOptiXScene& operator=(const FCarlaOptiXScene&) = delete;
	~FCarlaOptiXScene();

	constexpr auto GetOptixInstance() { return OptixInstance; }

};
