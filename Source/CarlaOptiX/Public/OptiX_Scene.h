#pragma once
#include "OptiX_Common.h"
#include "OptiX_StaticMesh.h"
#include "CarlaOptiX.h"
#include "OptiX_Scene.generated.h"


class CARLAOPTIX_API FCarlaOptiXInstance;
class CARLAOPTIX_API ACarlaOptiXScene;



class CARLAOPTIX_API FCarlaOptiXScene
{
	friend class ACarlaOptiXScene;

	FCarlaOptiXInstance* OptixInstance;
	OptixTraversableHandle GeometryAccelerationStructure;
	FOptixDeviceBuffer GASBuffer;

	std::vector<FCarlaOptiXStaticMesh> StaticMeshes;

	OptixModuleCompileOptions ModuleCompileOptions;
	OptixPipelineLinkOptions PipelineLinkOptions;
	OptixProgramGroupOptions ProgramGroupOptions;
	OptixPipeline Pipeline;
	OptixShaderBindingTable ShaderBindingTable;
	OptixFunctionTable FunctionTable;
	OptixDeviceContext DeviceContext;

	void EnumerateBuildInputs(
		std::vector<OptixBuildInput>& OutBuildInputs,
		std::vector<CUdeviceptr>& OutPointers,
		std::vector<unsigned>& OutFlags);

	void BuildGAS();
	void AddSceneStaticMeshes(UWorld* Source);

public:

	FCarlaOptiXScene();
	FCarlaOptiXScene(
		FCarlaOptiXInstance* OptixInstance);
	FCarlaOptiXScene(const FCarlaOptiXScene&) = delete;
	FCarlaOptiXScene& operator=(const FCarlaOptiXScene&) = delete;
	FCarlaOptiXScene(FCarlaOptiXScene&&);
	FCarlaOptiXScene& operator=(FCarlaOptiXScene&&);
	~FCarlaOptiXScene();

	void UpdateFromWorld(UWorld* Source);

	constexpr bool IsValid() const { return OptixInstance != nullptr; }
	constexpr auto& GetOptixInstance() { return *OptixInstance; }

};



UCLASS()
class CARLAOPTIX_API ACarlaOptiXScene :
	public AActor
{
	GENERATED_BODY()
public:

	ACarlaOptiXScene(const FObjectInitializer&);

	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	void UpdateFromWorld(UWorld* Source);

	UFUNCTION(BlueprintCallable)
	void UpdateFromCurrentWorld();

private:

	FCarlaOptiXScene Implementation;

};