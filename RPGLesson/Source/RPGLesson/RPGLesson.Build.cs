// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RPGLesson : ModuleRules
{
	public RPGLesson(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay","AIModule", "UMG", "Voxel"  });
		PrivateDependencyModuleNames.AddRange(new string[]{"Slate", "SlateCore"});
		bEnableUndefinedIdentifierWarnings = false;
	}
}
