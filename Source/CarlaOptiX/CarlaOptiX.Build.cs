// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using EpicGames.Core;
using System;
using System.IO;
using System.Diagnostics;

public class CarlaOptiX : ModuleRules
{

  [CommandLine("-optix-path")]
  string NVOptiXSDKPath = Environment.GetEnvironmentVariable(
    "OPTIX_TOOLKIT_PATH") ?? "";

  [CommandLine("-cuda-path")]
  string CUDAPath = Environment.GetEnvironmentVariable(
    "CUDA_PATH") ?? "";



  public CarlaOptiX(ReadOnlyTargetRules Target) : base(Target)
  {
    PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
    CppStandard = CppStandardVersion.Latest;

    Console.WriteLine("NVIDIA OptiX SDK Path = \"" + NVOptiXSDKPath + "\"");
    Console.WriteLine("CUDA SDK Path = \"" + CUDAPath + "\"");

    if (!Directory.Exists(NVOptiXSDKPath))
      throw new DirectoryNotFoundException();

    if (!Directory.Exists(CUDAPath))
      throw new DirectoryNotFoundException();
    
    var DependenciesPath = Path.Combine(PluginDirectory, "Dependencies");
    if (!Directory.Exists(DependenciesPath))
      Directory.CreateDirectory(DependenciesPath);

    var NVOptixIncludePath = Path.Combine(NVOptiXSDKPath, "include");
    var CUDAIncludePath = Path.Combine(CUDAPath, "include");
    var CUDALibPath = Path.Combine(CUDAPath, "lib", "x64");
    var CUDALibraryPath = Path.Combine(CUDALibPath, "cuda.lib");

    PublicIncludePaths.AddRange(new string[]
    {
      DependenciesPath,
      NVOptixIncludePath,
      CUDAIncludePath
    	// ... add public include paths required here ...
    });

    PrivateDefinitions.Add("CARLA_OPTIX_DEBUG_CHECKS"); // @TODO REMOVE
    PrivateDefinitions.Add("CARLA_OPTIX_VERBOSE"); // @TODO REMOVE

    PrivateIncludePaths.AddRange(new string[]
    {
    	// ... add other private include paths required here ...
    });

    PublicAdditionalLibraries.AddRange(new string[]
    {
      CUDALibraryPath
    });


    PublicDependencyModuleNames.AddRange(new string[]
    {
      "Core"
    });

    PrivateDependencyModuleNames.AddRange(new string[]
    {
      "CoreUObject",
      "Engine",
      "Slate",
      "SlateCore"
    });


    DynamicallyLoadedModuleNames.AddRange(new string[]
    {
    	// ... add any modules that your module loads dynamically here ...
    });
  }
}
