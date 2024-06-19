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
  [CommandLine("-optix-path")]
  string NVOptiXSDKPath = Environment.GetEnvironmentVariable(
    "OPTIX_TOOLKIT_PATH") ?? "";

  [CommandLine("-cuda-path")]
  string CUDAPath = Environment.GetEnvironmentVariable(
    "CUDA_PATH") ?? "";

  [CommandLine("-optix-debug")]
  bool OptiXDebug = true;



  private string PluginSourcePath;
  private string KernelSourcePath;
  private string KernelStringPath;
  private string DependenciesPath;
  private string NVOptixIncludePath;
  private string CUDAIncludePath;
  private string CUDALibPath;
  private string CUDALibraryPath;



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



  private void CompileCUDAToPTX(
    string Source,
    string OptiXIncludePath)
  {
    var Destination = Path.Combine(
      KernelStringPath,
      Path.GetFileNameWithoutExtension(Source) + ".ptx");
    
      var NVCCProcess = new Process();
    var Info = new ProcessStartInfo();
    var OptimizationFlags = OptiXDebug ? "-O0" : "-O3";
    var DebugFlags = OptiXDebug ? "-g -G" : "";

    Info.FileName = $"{CUDAPath}/bin/nvcc";
    Info.Arguments = $" {OptimizationFlags} {DebugFlags} --ptx {Source} -o {Destination} -I\"{OptiXIncludePath}\"";
    NVCCProcess.StartInfo = Info;
    Console.WriteLine($"Running \"{Info.FileName} {Info.Arguments}\"");
    NVCCProcess.Start();
    NVCCProcess.WaitForExit();
    if (NVCCProcess.ExitCode != 0)
    {
      if (File.Exists(Destination))
        File.Delete(Destination);
      throw new Exception($"Failed to compile \"{Destination}\".");
    }
  }



  public CarlaOptiX(ReadOnlyTargetRules Target) : base(Target)
  {
    PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
    CppStandard = CppStandardVersion.Latest;

    PluginSourcePath = Path.Combine(PluginDirectory, "Source", "CarlaOptiX");
    KernelSourcePath = Path.Combine(PluginSourcePath, "Kernel");
    KernelStringPath = Path.Combine(PluginSourcePath, "Private", "OptiX-IR");
    DependenciesPath = Path.Combine(PluginDirectory, "Dependencies");
    NVOptixIncludePath = Path.Combine(NVOptiXSDKPath, "include");
    CUDAIncludePath = Path.Combine(CUDAPath, "include");
    CUDALibPath = Path.Combine(CUDAPath, "lib", "x64");
    CUDALibraryPath = Path.Combine(CUDALibPath, "cuda.lib");

    Console.WriteLine($"NVIDIA OptiX SDK Path = \"{NVOptiXSDKPath}\"");
    Console.WriteLine($"CUDA SDK Path = \"{CUDAPath}\"");

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

    if (OptiXDebug)
      PublicDefinitions.Add("CARLA_OPTIX_DEBUG");
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



    var ModuleList =
@"#pragma once
#include ""Optix_Common.h""
";
    foreach (var Kernel in EnumerateCUDAKernels(KernelSourcePath))
      CompileCUDAToPTX(Kernel, NVOptixIncludePath);

    File.WriteAllText(
      Path.Combine(PluginSourcePath, "Private", "BuiltinKernelList.h"),
      ModuleList);
  }
}
