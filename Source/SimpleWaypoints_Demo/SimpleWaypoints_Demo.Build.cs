// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SimpleWaypoints_Demo : ModuleRules
{
	public SimpleWaypoints_Demo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
