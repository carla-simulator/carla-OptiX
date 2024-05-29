// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;
using EpicGames.Core;
using System.IO;

public class CarlaOptiX : ModuleRules
{

  [CommandLine("-optix-path")]
  string NVOptiXSDKPath = Environment.GetEnvironmentVariable(
    "CARLA_NVIDIA_OPTIX_SDK_PATH");



  public CarlaOptiX(ReadOnlyTargetRules Target) : base(Target)
  {
    PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

    var NVOptixIncludePath = Path.Combine(NVOptiXSDKPath, "include");

    if (!Directory.Exists(NVOptiXSDKPath))
      throw new DirectoryNotFoundException();

    if (false)
    {
      var CUDAPath = Environment.GetEnvironmentVariable("CUDA_PATH");
      var CUDAIncludePath = Path.Combine(CUDAPath, "include");
      if (!Directory.Exists(CUDAPath))
        throw new DirectoryNotFoundException();
      Console.WriteLine("CUDA SDK Path = \"" + CUDAPath + "\"");
    }

    Console.WriteLine("NVIDIA OptiX SDK Path = \"" + NVOptiXSDKPath + "\"");

    PublicIncludePaths.AddRange(
      new string[]
      {
        NVOptixIncludePath
				// ... add public include paths required here ...
			});


    PrivateIncludePaths.AddRange(
      new string[] {
				// ... add other private include paths required here ...
			}
      );


    PublicDependencyModuleNames.AddRange(
      new string[]
      {
        "Core",
        "CUDA",
				// ... add other public dependencies that you statically link with here ...
			}
      );


    PrivateDependencyModuleNames.AddRange(
      new string[]
      {
        "CoreUObject",
        "Engine",
        "Slate",
        "SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
      );


    DynamicallyLoadedModuleNames.AddRange(
      new string[]
      {
				// ... add any modules that your module loads dynamically here ...
			}
      );
  }
}
