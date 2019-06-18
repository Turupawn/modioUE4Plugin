// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class modio : ModuleRules
{
	private String modio_directory = "mod.io-sdk-v0.11.2";

	private string ModulePath
	{
		get { return ModuleDirectory; }
	}

	private string ProjectPath
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", "..", "..")); }
	}
	
	private string ThirdPartyPath
	{
		get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
	}

	public modio(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDefinitions.Add("JSON_NOEXCEPTION");
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// For beeing able to declare something public inside of the module, but private to outsiders
		PrivateDefinitions.Add( "MODIO_PACKAGE=1" );

		PrivateDependencyModuleNames.AddRange(new string[] { });

		LoadModio(Target);

		bEnableExceptions = true;
		// Made sure to disable unity builds, as exclusion of some files causes the project to explode
		// this was we atleast get deterministic builds even if they are slower
		MinSourceFilesForUnityBuildOverride = 256;
		
		PublicIncludePaths.AddRange(
			new string[] {
				//"modio/Public"
				/*,
				"Runtime/Core/Public/Modules/",
				"Editor/UnrealEd/Classes/Factories",
				"Editor/UnrealEd/Classes",
				"C:/Users/antar/Documents/Unreal Projects/ModioContainer21/Plugins/modio/Intermediate/Build/Win64/UE4Editor/Inc/modio",
				"C:/Program Files/Epic Games/UE_4.21/Engine/Intermediate/Build/Win64/UE4Editor/Inc/UnrealEd"
				*/
				// ... add public include paths required here ...
			}
			);
		
		PrivateIncludePaths.AddRange(
			new string[] {
				//"modio/Private",
				// ... add other private include paths required here ...
			}
			);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"Projects",
				"Int64"
				// ... add other public dependencies that you statically link with here ...
			}
			);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
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

	public bool LoadModio(ReadOnlyTargetRules Target)
	{
		bool isLibrarySupported = false;
		if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
		{
			isLibrarySupported = true;
			
			string LibrariesPath = Path.Combine(ThirdPartyPath, modio_directory, "lib", "visualc++", "x64");
			string DLLPath = Path.Combine(ThirdPartyPath, modio_directory, "bin", "visualc++", "x64");

			PublicLibraryPaths.Add(LibrariesPath);
			PublicAdditionalLibraries.Add("modio.lib");
			RuntimeDependencies.Add(Path.Combine(DLLPath, "modio.dll"));

			string ProjectBinariesDirectory = Path.Combine(ProjectPath, "Binaries", "Win64");
			if (!Directory.Exists(ProjectBinariesDirectory))
				System.IO.Directory.CreateDirectory(ProjectBinariesDirectory);
			
			string ModioDLLDestination = System.IO.Path.Combine(ProjectBinariesDirectory, "modio.dll");
			CopyFile(Path.Combine(DLLPath, "modio.dll"), ModioDLLDestination);
			PublicDelayLoadDLLs.AddRange(new string[] { "modio.dll" });
		}
		
		if (isLibrarySupported)
		{
			string ModioIncludePath = Path.Combine(ThirdPartyPath, modio_directory, "include");
			string AdditionalDependenciesPath = Path.Combine(ThirdPartyPath, modio_directory, "additional_dependencies");
			string MiniZPath = Path.Combine(ThirdPartyPath, modio_directory, "include/dependencies/miniz");
			PublicIncludePaths.Add(ModioIncludePath);
			PublicIncludePaths.Add(AdditionalDependenciesPath);
			PublicIncludePaths.Add(MiniZPath);
		}
		
		return isLibrarySupported;
	}

	private void CopyFile(string source, string dest)
	{
		System.Console.WriteLine("Copying {0} to {1}", source, dest);
		if (System.IO.File.Exists(dest))
		{
			System.IO.File.SetAttributes(dest, System.IO.File.GetAttributes(dest) & ~System.IO.FileAttributes.ReadOnly);
		}
		try
		{
			System.IO.File.Copy(source, dest, true);
		}
		catch (System.Exception ex)
		{
			System.Console.WriteLine("Failed to copy file: {0}", ex.Message);
		}
	}
}
