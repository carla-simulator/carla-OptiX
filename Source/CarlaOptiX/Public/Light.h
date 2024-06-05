#pragma once
#include "OptiXHeaders.h"



struct CARLAOPTIX_API FOptixPointLightInfo
{
	FTransform Transform;
	FVector3d Color;
	float Intensity;
	float Range;
};



struct CARLAOPTIX_API FOptixSpotLightInfo
{
	FTransform Transform;
	FVector3d Color;
	float Intensity;
	float Range;
	float InnerConeAngle;
	float OuterConeAngle;
};
