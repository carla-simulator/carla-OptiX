#include "OptiX_Scene.h"
#include "OptiX_Instance.h"
#include "Kismet/GameplayStatics.h"

FCarlaOptiXScene::FCarlaOptiXScene() :
	OptixInstance(nullptr)
{
}

FCarlaOptiXScene::FCarlaOptiXScene(FCarlaOptiXInstance* OptixInstance) :
	OptixInstance(OptixInstance)
{
}

void FCarlaOptiXScene::AddSceneStaticMeshes(UWorld* Source)
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(
		Source,
		AActor::StaticClass(),
		Actors);
	TArray<UActorComponent*> Components;
	for (auto Actor : Actors)
	{
		auto ActorComponents = Actor->GetComponents();
		Components.Reserve(Components.Num() + ActorComponents.Num());
		for (auto Component : ActorComponents)
			Components.Add(Component);
	}
	Actors.Empty();
	TArray<UStaticMesh*> Meshes;
	for (auto Component : Components)
		if (auto Mesh = Cast<UStaticMeshComponent>(Component); Mesh != nullptr)
			Meshes.Add(Mesh->GetStaticMesh());
	Components.Empty();
	for (auto Mesh : Meshes)
		StaticMeshes.Add(FCarlaOptiXStaticMesh(Mesh));
}

void FCarlaOptiXScene::UpdateFromWorld(UWorld* Source)
{
	AddSceneStaticMeshes(Source);
}



ACarlaOptiXScene::ACarlaOptiXScene(
	const FObjectInitializer& Initializer) :
	Super(Initializer)
{
}

void ACarlaOptiXScene::UpdateFromWorld(UWorld* Source)
{
	AddSceneStaticMeshes(Source);
}

void ACarlaOptiXScene::UpdateFromCurrentWorld()
{
	auto World = GetWorld();
	check(World);
	UpdateFromWorld(World);
}
