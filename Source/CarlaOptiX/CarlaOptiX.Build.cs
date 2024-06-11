// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using EpicGames.Core;
using System;
using System.IO;
using System.Diagnostics;
using System.Collections.Generic;
using UnrealBuildTool.Rules;

public class CarlaOptiX : ModuleRules
{
  private struct KernelModuleList
  {
    public string RayGen;
    public string AnyHit;
    public string ClosestHit;
    public string Intersection;
    public string Miss;
  }

  [CommandLine("-optix-path")]
  string NVOptiXSDKPath = Environment.GetEnvironmentVariable(
    "OPTIX_TOOLKIT_PATH") ?? "";

  [CommandLine("-cuda-path")]
  string CUDAPath = Environment.GetEnvironmentVariable(
    "CUDA_PATH") ?? "";

  private static List<string> EnumerateCUDAKernels(string InPath)
  {
    var Files = new List<string>();
    foreach (var File in Directory.GetFiles(InPath))
    {
		if (Path.GetExtension(File) == ".cu")
        {
			Console.WriteLine("Found " + File);
			Files.Add(File);
		}
	}
    foreach (var SubDirectory in Directory.GetDirectories(InPath))
      Files.AddRange(EnumerateCUDAKernels(SubDirectory));
    return Files;
  }

  public CarlaOptiX(ReadOnlyTargetRules Target) : base(Target)
  {
    PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
    CppStandard = CppStandardVersion.Latest;
    
    var PluginSourcePath = Path.Combine(PluginDirectory, "Source", "CarlaOptiX");
    var KernelSourcePath = Path.Combine(PluginSourcePath, "Kernel");
    var KernelStringPath = Path.Combine(PluginSourcePath, "Private", "KernelStrings");
    var DependenciesPath = Path.Combine(PluginDirectory, "Dependencies");
    var NVOptixIncludePath = Path.Combine(NVOptiXSDKPath, "include");
    var CUDAIncludePath = Path.Combine(CUDAPath, "include");
    var CUDALibPath = Path.Combine(CUDAPath, "lib", "x64");
    var CUDALibraryPath = Path.Combine(CUDALibPath, "cuda.lib");

    Console.WriteLine("NVIDIA OptiX SDK Path = \"" + NVOptiXSDKPath + "\"");
    Console.WriteLine("CUDA SDK Path = \"" + CUDAPath + "\"");

    if (!Directory.Exists(NVOptiXSDKPath))
      throw new DirectoryNotFoundException();

    if (!Directory.Exists(CUDAPath))
      throw new DirectoryNotFoundException();

    if (!Directory.Exists(DependenciesPath))
      Directory.CreateDirectory(DependenciesPath);

    if (!Directory.Exists(KernelStringPath))
      Directory.CreateDirectory(KernelStringPath);

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

    var KernelMap = new Dictionary<string, KernelModuleList>();
    foreach (var Kernel in EnumerateCUDAKernels(KernelSourcePath))
    {
      var KernelSourceStringPath = Path.Combine(
          KernelStringPath,
          Path.GetFileName(Kernel)) + ".str";
      var SourceString = "R\"(";
      SourceString += File.ReadAllText(Kernel);
      SourceString += ")\"\n";
      File.WriteAllText(
        KernelSourceStringPath,
        SourceString);
      var Key = Path.GetFileName(Kernel);
      var DotIndex = Key.IndexOf('.');
      if (DotIndex == -1)
        DotIndex = Key.Length;
      var Extension = Key.Substring(DotIndex, Key.Length - DotIndex);
      Key = Key.Substring(0, DotIndex);
      KernelMap.TryAdd(Key, new KernelModuleList());
      var Value = KernelMap[Key];
      KernelSourceStringPath = KernelSourceStringPath.Replace('\\', '/');
      switch (Extension)
      {
        case ".ah.cu":
          Value.AnyHit = KernelSourceStringPath;
          break;
        case ".ch.cu":
          Value.ClosestHit = KernelSourceStringPath;
          break;
        case ".i.cu":
          Value.Intersection = KernelSourceStringPath;
          break;
        case ".m.cu":
          Value.Miss = KernelSourceStringPath;
          break;
        case ".rg.cu":
          Value.RayGen = KernelSourceStringPath;
          break;
        default:
          break;
      }
      KernelMap[Key] = Value;
    }

    var ModuleList = "#pragma once \n";
    foreach (var Entry in KernelMap)
    {
      ModuleList +=
$@"struct F{Entry.Key}ModuleGroup
{{
static constexpr char RayGen[] =
#include ""{Entry.Value.RayGen}""
;
static constexpr char AnyHit[] =
#include ""{Entry.Value.AnyHit}""
;
static constexpr char ClosestHit[] =
#include ""{Entry.Value.ClosestHit}""
;
static constexpr char Intersection[] =
#include ""{Entry.Value.Intersection}""
;
static constexpr char Miss[] =
#include ""{Entry.Value.Miss}""
;
}};";
      ModuleList += '\n';
    }
    File.WriteAllText(Path.Combine(PluginSourcePath, "Private", "BuiltinKernelList.h"), ModuleList);
  }
}
