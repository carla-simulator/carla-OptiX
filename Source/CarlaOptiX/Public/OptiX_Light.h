#pragma once
#include "OptiX_Common.h"
#include "OptiX_Light.generated.h"



USTRUCT(BlueprintType)
struct CARLAOPTIX_API FCarlaOptixPointLightInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	FLinearColor Color;

	UPROPERTY()
	float Intensity;

	UPROPERTY()
	float Range;
};



USTRUCT(BlueprintType)
struct CARLAOPTIX_API FCarlaOptixSpotLightInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	FLinearColor Color;

	UPROPERTY()
	float Intensity;

	UPROPERTY()
	float Range;

	UPROPERTY()
	float InnerConeAngle;

	UPROPERTY()
	float OuterConeAngle;
};
