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
	TArray<FCarlaOptiXStaticMesh> StaticMeshes;

	void AddSceneStaticMeshes(UWorld* Source);

public:

	FCarlaOptiXScene();
	FCarlaOptiXScene(
		FCarlaOptiXInstance* OptixInstance);
	FCarlaOptiXScene(const FCarlaOptiXScene&) = delete;
	FCarlaOptiXScene& operator=(const FCarlaOptiXScene&) = delete;
	~FCarlaOptiXScene() = default;

	void UpdateFromWorld(UWorld* Source);

	constexpr bool IsValid() const { return OptixInstance != nullptr; }
	constexpr auto& GetOptixInstance() { return *OptixInstance; }

};



UCLASS()
class CARLAOPTIX_API ACarlaOptiXScene :
	public AActor,
	public FCarlaOptiXScene
{
	GENERATED_BODY()
public:

	ACarlaOptiXScene(const FObjectInitializer&);

	UFUNCTION(BlueprintCallable)
	void UpdateFromWorld(UWorld* Source);

	UFUNCTION(BlueprintCallable)
	void UpdateFromCurrentWorld();

};