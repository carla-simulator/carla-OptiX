#include "OptiX_SceneCaptureComponent.h"
#include "OptiX_Module.h"
#include "Engine/TextureRenderTarget2D.h"



ASceneCaptureComponentNVOptiX::ASceneCaptureComponentNVOptiX(
	const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	RenderTargetUE = CreateDefaultSubobject<UTextureRenderTarget2D>(
		TEXT("RenderTarget"));
}

void ASceneCaptureComponentNVOptiX::BeginPlay()
{
}

void ASceneCaptureComponentNVOptiX::EndPlay(
	const EEndPlayReason::Type EndPlayReason)
{
}

void ASceneCaptureComponentNVOptiX::RenderScene()
{
}
